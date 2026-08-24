# Session Manager Integration Guide

This document describes exactly how `http::SessionManager` integrates into the
connection lifecycle, which module owns each responsibility, and what code
runs at each state transition.

---

## 1. Where SessionManager Fits in the Architecture

SessionManager is a singleton service that lives conceptually inside the
`http` module. It does not participate in I/O, does not know about sockets,
and does not know about `epoll`. It is consulted by the `core` module
(specifically the `Connection` orchestrator) during the `PROCESSING` state,
and its output (a `Set-Cookie` header) is written by the `http` response
construction code.

```
io      → delivers bytes only, no knowledge of sessions
core    → Connection asks SessionManager: "is this session valid?"
http    → Parser extracts the cookie; ResponseHandler writes Set-Cookie
utils   → (optional) cookie string parsing helpers
```

SessionManager does not alter your connection state machine
(`READING → PARSING → PROCESSING → WRITING`). It is additional data that
flows through the existing states, the same way routing or configuration
lookups do.

---

## 2. The Improved Interface

```cpp
namespace http {

class SessionManager {
public:
    static SessionManager& instance();

    // Initialization
    void init(const std::string& cookie_name, size_t timeout_seconds);
    bool is_initialized() const;

    // Session lifecycle
    std::string create_session();
    bool has_session(const std::string& id) const;
    void touch_session(const std::string& id);
    void delete_session(const std::string& id);

    // Session data storage
    void set_session_data(const std::string& id,
                           const std::string& key,
                           const std::string& value);
    std::string get_session_data(const std::string& id,
                                  const std::string& key) const;
    bool has_session_data(const std::string& id,
                           const std::string& key) const;

    // Maintenance
    void cleanup();
    size_t get_session_count() const;

    // Config access
    std::string get_cookie_name() const;

private:
    SessionManager();
    ~SessionManager();
    SessionManager(const SessionManager&) = delete;
    SessionManager& operator=(const SessionManager&) = delete;

    std::string generate_session_id();

    std::string cookie_name;
    size_t timeout_seconds;
    bool initialized;

    struct SessionData {
        time_t creation_time;
        time_t last_touch;
        std::map<std::string, std::string> data;
    };

    std::map<std::string, SessionData> sessions;
};

}  // namespace http
```

---

## 3. Integration Steps, In Execution Order

### Step 1 — Startup: `main()`

**Owner:** `main`
**Runs:** once, before the event loop starts

Call `init()` right after configuration is loaded and validated, and before
any sockets are accepted. This guarantees the cookie name and timeout are
set before any connection could possibly need them.

```cpp
int main(int argc, char** argv) {
    Config config = load_config(argv[1]);

    http::SessionManager::instance().init("WEBSERV_SESSION", 1800);
    assert(http::SessionManager::instance().is_initialized());

    EventLoop loop(config);
    loop.run();
}
```

Why here: initialization is a one-time setup concern, identical in spirit to
opening listening sockets or loading MIME type tables. It must never be
deferred to first-use, or you risk a race between "first request arrives"
and "SessionManager configured."

---

### Step 2 — Cookie Extraction: after `PARSE_COMPLETE`

**Owner:** `http` (helper function, could live in `utils` if fully stateless)
**Runs:** once per request, immediately after the parser signals
`PARSE_COMPLETE` and before entering `PROCESSING`

The parser has already validated HTTP structure. Now you pull the raw
`Cookie` header value out of the parsed request and extract the specific
cookie you care about.

```cpp
namespace http {

std::string extract_cookie_value(const HTTPRequest& req,
                                   const std::string& cookie_name) {
    auto it = req.headers.find("cookie");
    if (it == req.headers.end()) {
        return "";
    }

    const std::string& cookies = it->second;
    std::string target = cookie_name + "=";

    size_t pos = cookies.find(target);
    if (pos == std::string::npos) {
        return "";
    }

    size_t start = pos + target.length();
    size_t end = cookies.find(';', start);

    if (end == std::string::npos) {
        return cookies.substr(start);
    }
    return cookies.substr(start, end - start);
}

}  // namespace http
```

Why here: this is pure protocol-level string parsing — no different from
reading `Content-Length`. It belongs next to the rest of your header
handling, not inside `Connection`, which should stay a transport-level
orchestrator.

---

### Step 3 — Session Validation: start of `PROCESSING`

**Owner:** `core::Connection`
**Runs:** once per request, at `PROCESSING` state entry

`Connection` asks `SessionManager` whether the extracted ID corresponds to a
live, non-expired session. This is the only place `Connection` touches
sessions directly — it is a validation lookup, not a data manipulation.

```cpp
void Connection::process_request() {
    HTTPRequest& req = parser.get_request();

    std::string cookie_name = http::SessionManager::instance().get_cookie_name();
    std::string sid = http::extract_cookie_value(req, cookie_name);

    bool valid = !sid.empty() &&
                 http::SessionManager::instance().has_session(sid);

    if (valid) {
        http::SessionManager::instance().touch_session(sid);
    }

    current_session_id = sid;
    session_valid = valid;

    state = PROCESSING;
}
```

Why here: `PROCESSING` is defined in your state machine as the phase that
"resolves configuration context, routes the request, and generates a
response." Session validity is exactly that kind of contextual resolution —
it happens once, before routing, and its result (`session_valid`,
`current_session_id`) is cached on the connection for the rest of the
request's lifetime.

Why `touch_session()` happens here and not later: refreshing the timeout
should reflect "the client made a request," which is true the moment
processing begins, regardless of what the handler ultimately does.

---

### Step 4 — Application Logic Using Session Data (optional)

**Owner:** whatever handler is invoked (e.g. `CGIHandler`, `StaticFileHandler`,
a future `LoginHandler`)
**Runs:** during `PROCESSING`, after validation, only if the handler needs it

If your routing logic determines this request needs session-scoped data
(login state, cart contents, form state), it reads or writes through
`SessionManager`, keyed by `current_session_id`.

```cpp
// Example: a login handler
void LoginHandler::handle(const HTTPRequest& req, Connection& conn) {
    if (credentials_valid(req)) {
        http::SessionManager::instance().set_session_data(
            conn.current_session_id, "user_id", lookup_user_id(req));
    }
}

// Example: a handler that needs to know if the user is logged in
bool is_logged_in(Connection& conn) {
    return http::SessionManager::instance().has_session_data(
        conn.current_session_id, "user_id");
}
```

Why here: this is business logic specific to your routes, not something
`Connection` or the transport layer should know about. `Connection` only
ever knows "is there a valid session," not what the session contains.

---

### Step 5 — Response Generation: `Set-Cookie` Header

**Owner:** `http::ResponseHandler`
**Runs:** once per request, while building the `Response` object, before
transitioning to `WRITING`

If the connection did **not** have a valid session coming in, a new one is
created here and written back to the client via `Set-Cookie`. If it already
had a valid session, nothing needs to be sent — the client already holds
the cookie.

```cpp
Response ResponseHandler::build_response(const HTTPRequest& req,
                                          Connection& conn) {
    Response resp = /* ... normal response construction ... */;

    if (!conn.session_valid) {
        std::string new_sid = http::SessionManager::instance().create_session();

        std::string cookie_header =
            http::SessionManager::instance().get_cookie_name() + "=" + new_sid +
            "; Path=/; HttpOnly; Max-Age=1800";

        resp.add_header("Set-Cookie", cookie_header);
        conn.current_session_id = new_sid;
    }

    return resp;
}
```

Why here: `Set-Cookie` is a response header like any other (`Content-Type`,
`Content-Length`), so it belongs in the same place those are added. The
`Connection` never constructs headers directly — that responsibility stays
inside `http`.

---

### Step 6 — Sending the Response

**Owner:** `core::Connection` (`WRITING` state), `io` (actual `write()` calls)
**Runs:** during the normal write loop

No special handling is needed. The `Set-Cookie` header is already part of
the serialized response buffer by the time `WRITING` begins. It is sent
byte-for-byte along with the status line, other headers, and body, following
your existing edge-triggered write loop (`write()` until `EAGAIN` or buffer
drained).

Why here: sessions must remain invisible to the transport layer. If
`Connection` or `io` had to special-case cookie bytes, that would violate
the "Connection is pure transport" boundary you've already established for
this project.

---

### Step 7 — Explicit Logout (optional but recommended)

**Owner:** a route/handler dedicated to logout (e.g. `POST /logout`)
**Runs:** on-demand, whenever a client requests it

```cpp
void LogoutHandler::handle(const HTTPRequest& req, Connection& conn) {
    if (!conn.current_session_id.empty()) {
        http::SessionManager::instance().delete_session(conn.current_session_id);
    }
    // Response should also instruct the browser to drop the cookie:
    // Set-Cookie: WEBSERV_SESSION=; Path=/; Max-Age=0
}
```

Why here: waiting for the timeout-based `cleanup()` to remove a session
after logout is a security gap — the old session ID would remain valid
until it naturally expires. Explicit deletion closes that gap immediately.

---

### Step 8 — Periodic Cleanup: inside the Event Loop

**Owner:** `core` (specifically `EventLoop::run()`)
**Runs:** repeatedly, once per interval (e.g. every 60 seconds), between
`epoll_wait()` cycles

```cpp
void EventLoop::run() {
    time_t last_cleanup = time(NULL);
    const int CLEANUP_INTERVAL = 60;

    while (running) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, 1000);

        // ... dispatch events to connections as usual ...

        time_t now = time(NULL);
        if (now - last_cleanup >= CLEANUP_INTERVAL) {
            http::SessionManager::instance().cleanup();
            last_cleanup = now;
        }
    }
}
```

Why here: cleanup is a maintenance task unrelated to any single connection's
events, so it does not belong inside `Connection`. It must run in the same
single thread as the rest of the loop (no locking needed) and must not block
long enough to stall event dispatch — a simple map sweep over expired
entries is cheap enough to run inline.

---

## 4. Summary Table

| Step | Phase / State                     | Owner Module        | What Happens                                      |
|------|------------------------------------|----------------------|----------------------------------------------------|
| 1    | Server startup                     | `main`               | `init(cookie_name, timeout)`                       |
| 2    | After `PARSE_COMPLETE`             | `http` (utils)       | Extract cookie value from `Cookie` header          |
| 3    | `PROCESSING` entry                 | `core::Connection`   | `has_session()`, `touch_session()`                 |
| 4    | During `PROCESSING` (optional)     | Route handlers       | `set_session_data()` / `get_session_data()`        |
| 5    | Response construction              | `http::ResponseHandler` | `create_session()`, add `Set-Cookie` header     |
| 6    | `WRITING` state                    | `core` / `io`        | Send bytes as-is; no session-specific logic        |
| 7    | On-demand (logout route)           | Route handler        | `delete_session()`                                 |
| 8    | Inside `EventLoop::run()` loop     | `core`               | Periodic `cleanup()`                               |

---

## 5. Boundary Rules to Preserve

1. **`Connection` never builds headers.** It only reads `session_valid` /
   `current_session_id` and passes them along; `ResponseHandler` is the only
   place `Set-Cookie` is written.
2. **`io` never knows sessions exist.** Cookie bytes are just response bytes
   by the time they reach the write loop.
3. **`SessionManager` never touches sockets, parsing, or state machines.**
   It is a pure data store keyed by session ID, consulted by whoever needs
   it.
4. **Cleanup is time-based and passive.** It never runs as a reaction to a
   specific connection event — only as a periodic sweep in the main loop.
5. **Session IDs must be generated unpredictably** (see
   `generate_session_id()`) — sequential or guessable IDs allow session
   hijacking.

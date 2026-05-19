# BodyHandler — Response Body Architecture


## 1. Overview

This document describes the design of the response body pipeline, from the `produce()` call that serializes the HTTP response into a send buffer, down to the concrete body providers that stream different content types. Everything here was designed around three hard constraints from the project specification:

- the server must remain non-blocking at all times
- all socket and pipe I/O must go through `epoll`
- `errno` must never be checked after a read or write operation

---

## 2. The `produce()` Contract

`handler::produce(char* buffer, size_t max_size)` is responsible for serializing the entire HTTP response — status line, headers, and body — into the connection's send buffer across one or more calls.

### Return type

```cpp
ssize_t handler::produce(char* buffer, size_t max_size);
```

| Return value | Meaning |
|---|---|
| `n > 0` | `n` bytes written into buffer, more data remains |
| `0` | response fully serialized, nothing left to send |
| `-1` | unrecoverable error, close the connection |

The `ssize_t` return mirrors the UNIX `read()` / `write()` convention deliberately — the semantics are immediately recognizable to anyone reading the code.

### Serialization state machine

`produce()` advances through three internal states, tracked by `SerializeState`:

```
REQUEST_LINE → HEADERS → BODY
```

On each call, `produce()` fills the buffer as completely as possible. If the current state's data doesn't fill the buffer, it transitions to the next state and continues filling in the same call. The state machine does not yield between states unless the buffer is full.

### Persistent members

Because a single serialized response may span many `produce()` calls, the handler holds:

- `std::string _serialized` — the fully serialized status line and headers, built once
- `size_t _serial_offset` — how many bytes of `_serialized` have already been copied into the buffer across previous calls
- `SerializeState _state` — current position in the pipeline

The body is handled by an `IBodyProvider` instance which maintains its own internal cursor.

---

## 3. The `advance()` / `on_writeable()` Layer

`produce()` is never called directly by the EventLoop. The call chain is:

```
EventLoop::write_to_socket()
    → Connection::on_writeable(ssize_t bytes_sent)
        → Connection::advance()
            → handler::produce(buffer, SEND_CHUNK_SIZE)
```

### `advance()`

```cpp
bool Connection::advance(void) {
    ssize_t n = handler.produce(output_buff, SEND_CHUNK_SIZE);
    if (n < 0) {
        state = CLOSING;
        return false;
    }
    bytes_in_buff = n;
    return bytes_in_buff > 0;
}
```

`advance()` is the sole owner of the `-1` case from `produce()`. It sets `state = CLOSING` and returns `false`. It never touches `bytes_in_buff` on error, preventing the `-1` from being cast to a large `size_t`.

### `on_writeable()`

```cpp
bool Connection::on_writeable(ssize_t bytes_sent) {
    if (state == CLOSING) {
        close_after_write = true;
        return false;
    }
    if (bytes_sent < 0) {
        return false;           // write() returned -1, yield to epoll
    }
    sent_offset   += bytes_sent;
    bytes_in_buff -= bytes_sent;
    if (bytes_in_buff == 0) {
        sent_offset = 0;
        if (advance()) {
            return true;
        }
        if (close_after_write) {
            state = CLOSING;
        } else {
            state = READING;
        }
        return false;
    }
    return true;
}
```

`on_writeable()` is called after every `write()` in the EventLoop, including the very first call where `bytes_sent = 0`. Since `bytes_in_buff` is zero at the READING → WRITING transition, the first call always triggers `advance()` to fill the buffer, requiring no special initialization flag.

### EventLoop write loop

```cpp
void EventLoop::write_to_socket(Connection& conn) {
    ssize_t bytes_sent = 0;
    while (true) {
        if (!conn.on_writeable(bytes_sent))
            break;
        bytes_sent = ::write(conn.fd(),
                             conn.get_write_buff(),
                             conn.bytes_remaining());
    }
}
```

### errno rule

`errno` is never checked after any read or write operation. In edge-triggered epoll mode:

- `write()` returns `-1` → stop writing, yield back to epoll, which will fire `EPOLLOUT` again when the socket is ready
- `read()` returns `-1` → stop reading, yield back to epoll, which will fire `EPOLLIN` again when data is available
- Real errors surface as `EPOLLERR` or `EPOLLHUP` on the next epoll cycle

The `-1` return alone is sufficient to stop I/O. No errno distinction is needed.

---

## 4. `IBodyProvider` Interface

```cpp
namespace http {

class IBodyProvider {
public:
    virtual ~IBodyProvider() {}

    // Fill buff with up to max_size bytes.
    // Returns bytes written, 0 if finished, -1 on error.
    virtual ssize_t read(char* buff, size_t max_size) = 0;

    // True once all bytes have been produced.
    virtual bool finished() const = 0;
};

} // namespace http
```

The interface is intentionally minimal. `produce()` calls `read()` when `SerializeState == BODY` and checks `finished()` to transition to the done state.

### Buffer boundary at HEADERS → BODY transition

When `produce()` finishes copying the last header bytes and still has space remaining in the buffer, it immediately calls `body_provider->read(buffer + written, remaining)` in the same call. The provider's `max_size` parameter naturally Limits how many body bytes are read. No intermediate buffer or leftover tracking is needed — the provider's internal cursor advances by exactly the bytes read.

---

## 5. Response Types and Body Requirements

```
STATIC_FILE    → FileBodyProvider (file descriptor)
DIRECTORY      → temp file → FileBodyProvider (file descriptor)
CGI            → CGIRequestHandler → temp file → FileBodyProvider (file descriptor)
FILE_UPLOAD    → no body (headers only)
FILE_DELETE    → no body (headers only)
REDIRECT       → no body (headers only)
ERROR_RESPONSE → FileBodyProvider (dedicated error file on disk)
```

All response types that carry a body ultimately go through `FileBodyProvider`. There is no `MemoryBodyProvider` — the only in-memory case (`DIRECTORY`) writes to a temp file first.

---

## 6. `FileBodyProvider`

Handles all file-backed bodies. Regular disk I/O is exempt from the epoll requirement per the project specification — `read()` on disk files is called directly without readiness notification.

### Members

```cpp
int    _fd;
size_t _file_size;
size_t _bytes_sent;
```

### Construction

```cpp
// For STATIC_FILE and ERROR_RESPONSE
FileBodyProvider(const std::string& path);

// For DIRECTORY — temp file already written and opened
FileBodyProvider(int temp_fd, size_t size);
```

### `read()` behavior

Calls `::read(_fd, buff, max_size)`, advances `_bytes_sent`, returns bytes read. Returns `0` when `_bytes_sent >= _file_size`. Returns `(size_t)-1` on syscall error.

### Directory listing

The auto-index HTML is generated in full as a string, written to a temp file via `::write()`, and the temp file fd is passed to `FileBodyProvider`. The temp file is unlinked immediately after opening — the fd keeps the inode alive until the connection closes and the fd is released.

---

## 7. CGI Architecture

CGI is the only response type that involves a child process. It uses the existing `AEventHandler` polymorphism — the same interface the EventLoop uses for client sockets.

### `AEventHandler` interface

```cpp
namespace io {
class AEventHandler {
public:
    virtual void on_event(EventType event) = 0;
    virtual ~AEventHandler() {}
};
}
```

The EventLoop maps `fd → AEventHandler*` and dispatches blindly:

```cpp
AEventHandler* handler = static_cast<AEventHandler*>(events[i].data.ptr);
if (events[i].events & EPOLLIN)       handler->on_event(READABLE);
else if (events[i].events & EPOLLOUT) handler->on_event(WRITABLE);
else if (events[i].events & (EPOLLERR | EPOLLHUP)) handler->on_event(ERROR);
```

### `CGIRequestHandler`

A derived `AEventHandler` registered with epoll on the CGI process's stdout pipe fd.

**Members:**
- `int _pipe_fd` — non-blocking stdout pipe from CGI process
- `int _temp_fd` — temp file receiving CGI output
- `pid_t _cgi_pid` — for `kill()` and `waitpid()`
- `Connection& _conn` — the owning connection to wake up when done
- `EventLoop& _loop` — to modify epoll registrations
- `HandlerState _state` — `IDLE` or `ACTIVE`

**`on_event(READABLE)`:**

```
read chunk from pipe_fd into temp file
if read returns 0:
    pipe closed, CGI process finished
    waitpid(_cgi_pid)
    event_loop.remove(pipe_fd)
    event_loop.modify(conn_fd, EPOLLOUT)
    conn.set_body(temp_fd, bytes_written)
if read returns -1:
    yield, wait for next EPOLLIN
```

**`on_event(ERROR)`:**

```
kill(_cgi_pid, SIGKILL)
waitpid(_cgi_pid)
cleanup temp file
conn.set_error(502)
event_loop.remove(pipe_fd)
event_loop.modify(conn_fd, EPOLLOUT)
```

### CGI process setup

These are covered by prior minishell experience. Key CGI-specific requirements:

- `chdir()` to the CGI script's directory before `execve()` — the project requires it for relative path file access
- Build the environment variable array before `fork()` — key variables include `REQUEST_METHOD`, `QUERY_STRING`, `CONTENT_LENGTH`, `CONTENT_TYPE`, `PATH_INFO`, `SERVER_NAME`, `SERVER_PORT`, `SCRIPT_FILENAME`
- The stdout pipe must be set non-blocking with `fcntl(pipe_fd, F_SETFL, O_NONBLOCK)` before registering with epoll
- For POST requests, write the request body into the CGI process's stdin pipe

### Responsibility boundary

```
EventLoop   → fd readiness only (add / modify / remove)
CGIRequestHandler  → pipe draining, temp file writing, process reaping
Connection  → unaware of CGI internals, woken via epoll modify
```

The EventLoop never knows a CGI process was involved. It just sees an `EPOLLOUT` on a client socket fd.

---

## 8. CGI Timeout

The server uses a fixed timeout on `epoll_wait` rather than `-1`. This makes every iteration an implicit heartbeat sweep.

```cpp
int n = epoll_wait(epoll_fd, events, MAX_EVENTS, CGI_TIMEOUT_MS);

// handle the n returned events, mark each handler ACTIVE

// sweep all registered CGIRequestHandlers
for each cgi_handler in active_cgi_handlers:
    if cgi_handler.state == IDLE:
        kill(cgi_handler.pid, SIGKILL)
        waitpid(cgi_handler.pid)
        cleanup and send 504 to connection
    else:
        cgi_handler.state = IDLE   // reset for next sweep
```

A CGI process that produces no output for one full `CGI_TIMEOUT_MS` interval will have its handler still in `IDLE` on the next sweep and gets killed. The cost of occasional empty iterations when no CGI is running is negligible.

---

## 9. Connection State at READING → WRITING Transition

When the parser signals a complete request, the connection transitions from `READING` to `WRITING`. At this exact point the following must be reset to zero:

- `bytes_in_buff`
- `sent_offset`

The zero value of `bytes_in_buff` is the trigger that causes the first `on_writeable(0)` call in the write loop to immediately invoke `advance()`, which calls `produce()` for the first time and fills the buffer. No initialization flag is needed.

---

## 10. Design Invariants

- `produce()` is the only caller of `IBodyProvider::read()`
- `advance()` is the only caller of `produce()`
- `on_writeable()` is the only caller of `advance()`
- `EventLoop` is the only entity that calls `epoll_ctl`
- `CGIRequestHandler` communicates with `Connection` only through `EventLoop` fd registration
- `errno` is never checked after any I/O operation
- Regular disk file reads never go through epoll
- `CLOSING` is a terminal state — no further transitions occur once set

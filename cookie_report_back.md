# Cookie and Session Review

The basic session flow works: the counter persists, login creates a session,
and the profile endpoint recognizes a valid session. The implementation builds
successfully, but the following issues remain.

## 1. Logout returns `401 Unauthorized`

`LogoutHandler::handle()` deletes the session and returns `ERR_NONE`, but it
does not construct a response or call `responseReady()`.

Because the context remains in the processing state, the handler runs again.
The session has already been deleted, so the second execution treats the user
as unauthenticated and produces `401 Unauthorized`.

This behavior was reproduced with the running server.

Location: `src/http/pipeline/handlers/CookiesHandlers.cpp`, around line 182.

## 2. Session configuration is ignored

The configuration parser accepts these directives:

- `session_enabled`
- `session_cookie_name`
- `session_timeout`
- `session_store`

However, server startup initializes the singleton with hard-coded values:

```cpp
http::SessionManager::instance().init("WEBSERVER_SESSSION", 3600);
```

Consequences:

- `session_enabled off` has no effect.
- The configured cookie name is ignored.
- The configured timeout is ignored.
- The configured session store is unused.
- Every virtual server shares the same global session configuration.
- The emitted cookie name contains the typo `SESSSION`.

Location: `src/server/Server.cpp`, around line 32.

## 3. Logout does not remove the browser cookie

Deleting the server-side session is not enough. A successful logout response
should also expire the client cookie, for example:

```http
Set-Cookie: WEBSESSID=; Path=/; Max-Age=0; HttpOnly; SameSite=Lax
```

Otherwise, the browser continues sending a stale session ID.

Location: `src/http/pipeline/handlers/CookiesHandlers.cpp`, in
`LogoutHandler::handle()`.

## 4. Invalid login attempts create orphan sessions

`LoginHandler` creates a session before parsing and validating the username.
If the username is missing or invalid, the handler returns `ERR_BAD_REQUEST`,
but the newly created session remains in `SessionManager` until timeout
cleanup.

Repeated invalid requests can therefore grow the in-memory session map.
Validate the request first and create the session only after validation
succeeds.

Location: `src/http/pipeline/handlers/CookiesHandlers.cpp`, around lines 94-105.

## 5. Cookie header matching is not fully case-insensitive

HTTP header names are case-insensitive, but `extract_cookie_value()` only
recognizes exactly `Cookie` and `cookie`. Valid forms such as `COOKIE` or
`CoOkIe` are missed because the original header name is stored by the parser.

Normalize the header name before comparison or use a case-insensitive
comparison.

Location: `src/http/session/CookieUtils.cpp`, around lines 15-17.

## 6. Session ID generation has an insecure fallback

The normal path reads from `/dev/urandom`, which is appropriate, but failure
falls back to `std::rand()`. Collision retries also use `std::rand()`.
`std::rand()` is predictable and must not be used for authentication session
identifiers.

The `/dev/urandom` read result is also not checked for a complete 16-byte read.
If secure random generation fails, session creation should fail rather than
silently generating a predictable ID.

Location: `src/http/session/SessionManager.cpp`, around lines 169-203.

## 7. Authentication cookies lack defensive attributes

Cookies are currently emitted with only `Path=/`. Session cookies should at
least include:

- `HttpOnly`, to prevent JavaScript access.
- `SameSite=Lax` or a stricter policy, to reduce cross-site request attacks.
- `Secure` when the server is used over HTTPS.

Locations: the `Set-Cookie` construction in `CounterHandler` and
`LoginHandler` inside `src/http/pipeline/handlers/CookiesHandlers.cpp`.

## 8. Duplicate handler implementation files exist

Both of these files exist:

- `src/http/pipeline/handlers/CookieHandlers.cpp`
- `src/http/pipeline/handlers/CookiesHandlers.cpp`

Only `CookiesHandlers.cpp` is compiled by the Makefile. The unused duplicate is
already inconsistent with the compiled version and can easily cause future
fixes to be applied to the wrong file.

## Architecture note

Cookies themselves should not have request handlers. `CounterHandler`,
`LoginHandler`, `ProfileHandler`, and `LogoutHandler` are ordinary route
handlers that happen to use session state. Cookie parsing and session
validation belong in the session/context layer.

Calling their combined file `CookiesHandlers.cpp` is misleading. Separate
handler files or a name such as `SessionDemoHandlers.cpp` would make the
responsibilities clearer.

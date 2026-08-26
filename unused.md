# Unused Code Audit

This audit covers the current `webserv` executable. It was checked using source-reference searches, Makefile inclusion, Clang warnings, and linker dead-section reporting. The project built successfully during the audit.

There is no `baselib/` directory in the project, so `src/foundation/` was treated as its likely equivalent.

## Definitely unused by the current executable

- The deleted `src/base/memory/` allocator implementation was completely unused and was not compiled. Its deletion is justified.
- `src/http/pipeline/handlers/CookieHandlers.cpp` is an outdated, uncompiled duplicate of the compiled `CookiesHandlers.cpp`.
- `src/net/connection/connection_io.cpp` and `src/net/connection/connection_events.cpp` are empty translation units but are still compiled by the Makefile.
- `src/http/common/Date.hpp` is unused. It would also fail to compile because `Date::format()` declares two variables named `s` in the same scope.
- `src/http/common/Types.hpp` is unused.
- `src/server/limits.hpp` is completely unused.
- `src/runtime/poll/PollEventLoop.cpp` is neither compiled nor referenced. It is also unfinished: `nfds` is uninitialized and `add_fd()` increments it before indexing, producing an off-by-one error.
- `src/runtime/kqueue/KqueueEventPoller.hpp` is unused and contains an invalid empty `#include` directive.

## Unused code in `src/base/`

- `base::sizeof_file()` in `src/base/file.cpp`.
- `base::random_string()` in `src/base/random.cpp`.
- The following aliases in `src/base/types.hpp`:
  - `isize`
  - `i8`
  - `i16`
  - `i64`
  - `u8`
  - `u16`
  - `u32`
  - `u64`
  - `f32`
  - `f64`
- The following `base::io::Reader` API is unused by the executable:
  - `Reader(const std::string&)`
  - `Reader(i32, bool)`
  - `Reader(const char*, usize)`
  - `reset(i32, bool)`
- The following `base::io::Writer` API is unused by the executable:
  - `Writer(char*, usize)`
  - `Writer(i32, bool, char*, usize)`
  - `reset(char*, usize)`
  - `reset(i32, bool, char*, usize)`
  - `write(const std::string&)`
  - `writePtr()`
  - `offset()`
  - `remaining()`
  - `freeSpace()`
  - `advance()`
  - `commit()`
  - `type()`

The `Reader` and `Writer` entries are valid API implementations, but nothing in the current executable calls them. Remove them only if this project is not intended to expose or reuse them as a general-purpose library API.

## Unused code in `src/foundation/`

- `Buffer::full()`
- `Buffer::capacity()`
- `Buffer::reset()`
- `Buffer::pop()`
- `CStringArray::size()`
- `CStringArray::data()`
- `Timestamp(int)`
- All `Timestamp` comparison operators
- `Pipe::reset()`
- `UniqueFd::set()`

The following related runtime API is also unused:

- `AEventHandler::pause()`
- `AEventHandler::resume()`
- The associated `IOCtl` state

## Unused connection and socket state

The following `Connection` members are initialized but never read:

- `close_after_write`
- `last_activity_`
- `lifetime_`
- `info_`

The local and remote IP/port fields inside `ConnectionInfo` are collected and passed into `Connection`, but are never consumed. Only `ConnectionInfo::servers` is used.

The following `Socket` code is consequently unused or redundant:

- `host_`
- `port_`
- `servers() const`
- The `IpAddress` and `Port` aliases

## Other unused functions and APIs

- The configuration-printing block in `src/server/main.cpp`, ending in `config::printConfig()`. Its only call is commented out.
- `Server::abort()`
- The global `leaks(bool&)` helper; its call is commented out.
- `server_info::name`, `server_info::version`, and `server_info::info()`.
- `logger::Logger::enable()` and `logger::Logger::disable()`.
- `cgi::Channel::mark_closing()`.
- `cgi::Process::running()`.
- `cgi::Process::pid()`.
- `cgi::Process::status()`.
- `cgi::Process::status_code()`.
- `http::ResponseParser::finished()`.
- `http::CgiHandler::done()`.
- `http::CgiHandler::can_close()`.
- `http::ARequestHandler::eraseHeader()`.
- `http::SessionManager::is_initialized()`.
- `http::SessionManager::has_session_data()`.
- `http::SessionManager::get_session_count()`.

The following `http::Headers` operations are implemented but unused by the executable:

- Copy assignment
- `operator[]`
- `remove()`
- `has()`
- `replace()`
- `reserve()`
- `size()`
- Mutable `begin()` and `end()`

## Unused constants and enum members

The following constants in `src/http/limits.hpp` are unused:

- `BODY_PROGRESS_MIN_BYTES`
- `BODY_MIN_WAIT_SECONDS`
- `BODY_MIN_BYTES_PER_SECOND`

`Server::MaxListens` is unused. Within `Server::ServerErrors`, only `AllocFailed` is referenced; `SockFailed`, `ConfError`, `IOError`, and `ConnError` are not used.

## Test-only code

The following sources are not part of the production executable, but they are tests rather than accidental dead code:

- `src/http/Parser/unit_test/`
- `src/http/session/test_session.cpp`

The poll and kqueue implementations may similarly be retained if they are intended as future portability work, but neither participates in the current Linux/epoll build.

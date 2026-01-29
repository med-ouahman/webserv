# ARCH.md — Web Server Architecture (Draft)

## 1. Architectural Philosophy

This project follows an explicit, event-driven architecture designed for correctness, clarity, and robustness under non-blocking I/O constraints. Architectural decisions are treated as evolving design choices rather than rigid commitments and may be refined as implementation progresses.

The server avoids hidden global state and relies on explicit ownership and dependency flow to ensure predictable behavior and ease of reasoning.

---

## 2. High-Level Architecture and Module Interaction

The server is organized into distinct modules with clear responsibilities and strict dependency boundaries.

* `main` is responsible only for initialization and startup. It parses configuration, constructs top-level components, and transfers control to the event loop.
* `core` acts as the orchestration layer, coordinating connection lifecycles, enforcing the connection state machine, and driving request handling decisions.
* `io` encapsulates all operating system interactions, including socket management and I/O multiplexing. It reports readiness events without interpreting protocol semantics.
* `http` contains all HTTP protocol logic, including request parsing and response construction, independent of socket operations.
* `utils` provides stateless helper functionality shared across modules.

Dependencies are intentionally one-directional, with `core` acting as the only module allowed to coordinate between `io` and `http`.

---

## 3. Event Loop and I/O Multiplexing

The server employs a single-threaded, non-blocking event loop driven by operating system I/O multiplexing facilities. The event loop waits for socket readiness notifications and dispatches events to connection instances in accordance with their current state.

Socket interest (read or write) is determined strictly by the connection state machine. The event loop performs minimal work per event to ensure fairness and responsiveness across all active connections.

---

## 4. Connection State Machine

Each client connection is governed by a unified state machine defining all permitted behaviors and transitions.

Connections progress through well-defined states covering:

* acceptance
* reading
* parsing
* request processing
* response writing
* completion
* error handling
* closure

State transitions are driven exclusively by socket readiness events, parser outcomes, handler results, and fatal errors. Read and write phases are strictly separated to prevent interleaving and protocol violations.

---

## 5. HTTP Parsing Strategy

HTTP request parsing is implemented as an incremental, stateful process designed for non-blocking I/O. Each connection owns a dedicated HTTP parser instance that consumes raw bytes from a connection-managed read buffer.

The parser operates as a finite state machine and reports only factual outcomes: request completion, need for additional data, or parse errors. Parsing never assumes message completeness and preserves unread bytes to support keep-alive and pipelined requests.

---

## 6. Response Generation and Write-Side Flow Control

HTTP responses are generated after successful request parsing and serialized into a connection-owned write buffer prior to transmission.

Response data is written incrementally based on socket readiness events, with explicit tracking of write progress. Only one response is written per connection at a time. Upon completion, the server evaluates connection reuse rules and either resets the connection for subsequent requests or closes it.

---

## 7. Configuration and Server Context Model

Server configuration is parsed and fully validated during startup, producing an immutable runtime configuration model.

Configuration data is organized hierarchically into global, server, and location contexts. At request processing time, the server deterministically resolves the applicable context based on connection endpoint and request attributes. Resolved configuration data is shared as read-only input throughout the request lifecycle.

---

## 8. Request Routing and Handler Dispatch

Routing decisions are performed after request parsing and configuration context resolution. Routing is based solely on request attributes and configuration data and produces exactly one handling outcome per request.

The server selects between static file handling, CGI execution, or error handling using a deterministic evaluation order. Handlers operate independently of socket I/O and connection state management.

---

## 9. Filesystem Interaction and Path Resolution

URL-to-filesystem mapping is performed through a strict resolution process including path normalization, canonicalization, and confinement verification.

All filesystem access is restricted to configured root directories. Directory handling behavior, symbolic link policies, and MIME type resolution are explicitly governed by configuration. Filesystem errors are consistently translated into appropriate HTTP status codes.

---

## 10. CGI Execution Model

CGI requests are handled through controlled child process execution with explicit environment setup and strict enforcement of execution limits.

CGI input and output streams are integrated into the event loop to prevent blocking. Execution timeouts and output size constraints are enforced, and all resources are cleaned up deterministically to prevent leaks and orphaned processes.

---

## 11. Error Handling and Error Pages

Error handling is centralized and deterministic. Errors are classified into client, server, connection-level, and startup categories.

Recoverable errors are mapped to appropriate HTTP responses, with custom error pages resolved hierarchically. Internal details are never exposed to clients, and unrecoverable connection errors result in immediate cleanup and termination.

---

## 12. Timeouts, Limits, and Resource Management

The server enforces strict limits on connection lifetime, request sizes, concurrent resources, and CGI execution to ensure stability under load.

Timeouts and limits are evaluated during event loop execution, and violations result in fast, isolated failure without impacting unrelated connections.

---

## 13. Graceful Shutdown and Signal Handling

The server implements controlled shutdown behavior in response to termination signals. Shutdown proceeds in phases, stopping new connection acceptance, allowing in-flight requests to complete, and finally performing deterministic cleanup.

Signal handlers are minimal and communicate shutdown intent through shared state checked by the event loop.

Absolutely. Below is the **updated, consolidated `ARCH.md`**, rewritten to accurately reflect **everything we clarified and refined about connections**, without jumping ahead into unrelated areas.

This version is **clean, precise, and reviewer-safe**, and it removes earlier ambiguities around *connection vs request*, *epoll’s role*, and *state behavior*.

---

# ARCH.md — Web Server Architecture (Draft)

## 1. Architectural Philosophy

This project follows an explicit, event-driven architecture designed for correctness and clarity under non-blocking I/O constraints. The design prioritizes deterministic behavior, explicit ownership, and strict separation of concerns.

Architecture decisions documented here describe the current design intent and may evolve as implementation progresses. No global mutable state is relied upon; all behavior is driven through explicit objects and events.

---

## 2. High-Level Architecture and Module Interaction

The server is organized into distinct modules with strict responsibility boundaries:

* **`main`**
  Performs startup tasks only: configuration loading, validation, construction of top-level objects, and transfer of control to the event loop.

* **`core`**
  Owns the server’s orchestration logic. It manages connection lifecycles, enforces the connection state machine, and coordinates request processing.

* **`io`**
  Encapsulates all operating system interaction, including socket management and I/O multiplexing. It reports readiness events without interpreting protocol semantics.

* **`http`**
  Contains all HTTP protocol logic, including incremental request parsing and response construction, independent of socket operations.

* **`utils`**
  Provides stateless helper functionality shared across modules.

Dependencies are one-directional, with `core` acting as the sole coordinator between I/O and protocol logic.

---

## 3. Event Loop and I/O Multiplexing

The server uses a single-threaded, non-blocking event loop driven by an OS-level I/O multiplexing mechanism (e.g. `epoll`). The event loop waits for socket readiness notifications and dispatches events to the corresponding connection objects.

The event loop does not perform protocol logic or state transitions directly. Its responsibility is limited to:

* monitoring file descriptors,
* translating readiness signals into connection events,
* executing side effects requested by connection intent.

---

## 4. Connection Model

### 4.1 Definition

A **Connection** represents the server’s ownership and lifecycle management of a single client TCP communication channel.

A connection:

* is created when the server accepts a TCP connection,
* owns the socket file descriptor,
* persists across multiple I/O events,
* may handle **zero, one, or multiple HTTP requests** during its lifetime.

A connection is **not** a request, not a socket wrapper, and not an HTTP object. It is a coordination entity.

---

### 4.2 Connection vs Request

* A **Connection** is long-lived and transport-level.
* A **Request** is short-lived and protocol-level.

Multiple requests may be processed sequentially over a single connection when keep-alive is enabled. Request objects are created after successful parsing and destroyed after response generation.

---

## 5. Connection State Machine

Each connection is governed by a unified finite state machine. At any time, a connection is in exactly one state.

### Connection States

* `ACCEPTED`
* `READING`
* `PARSING`
* `PROCESSING`
* `READY_TO_WRITE`
* `WRITING`
* `WRITE_COMPLETE`
* `ERROR`
* `CLOSING`

State transitions are explicit and deterministic. Illegal transitions are treated as programming errors.

Read and write phases are strictly separated; a connection is never reading and writing at the same time.

---

## 6. Connection Events

Connection behavior is driven exclusively by **events**.

Events are facts, not decisions.

### External Events (produced by the event loop)

* `SOCKET_READABLE`
* `SOCKET_WRITABLE`
* `FATAL_ERROR`

### Internal Events (produced by the connection)

* `READ_SUCCESS`
* `READ_EOF`
* `READ_ERROR`
* `PARSE_NEED_MORE_BYTES`
* `PARSE_COMPLETE`
* `PARSE_ERROR`
* `PROCESSING_DONE`
* `WRITE_SUCCESS`
* `WRITE_ERROR`
* `CLOSE_REQUESTED`

All state transitions occur as a function of `(current_state, event)`.

---

## 7. State Entry Side Effects

State transitions determine **intent**.
Side effects execute that intent.

Side effects are associated with **state entry**, not with events directly.

### Summary of State Entry Intent

* **`ACCEPTED`**
  Initialize connection resources and register interest in read events.

* **`READING`**
  Register read interest and attempt to receive bytes when permitted.

* **`PARSING`**
  Consume available bytes using the HTTP parser without performing socket I/O.

* **`PROCESSING`**
  Resolve configuration context, route the request, and generate a response.

* **`READY_TO_WRITE`**
  Register write interest.

* **`WRITING`**
  Attempt to send response bytes incrementally.

* **`WRITE_COMPLETE`**
  Decide keep-alive behavior and either reset request state or initiate closure.

* **`ERROR`**
  Attempt to generate an error response if possible, otherwise prepare for closure.

* **`CLOSING`**
  Unregister the socket, close the file descriptor, and release all resources.

---

## 8. Event Production and Dispatch

Operating system readiness notifications are treated as **permission signals**, not outcomes.

The event loop translates:

* `EPOLLIN` → `SOCKET_READABLE`
* `EPOLLOUT` → `SOCKET_WRITABLE`
* `EPOLLERR / EPOLLHUP` → `FATAL_ERROR`

Connections perform the permitted action and then emit internal outcome events based on the result. State transitions occur only through these events.

epoll notifications never directly modify connection state.

---

## 9. Request Lifecycle Within a Connection

A connection may repeatedly execute the following cycle:

```
READ ↔ PARSE → PROCESS → WRITE
                ↓
           keep-alive decision
                ↓
         READ or CLOSING
```

Keep-alive behavior is determined during parsing based on HTTP semantics and server policy. If keep-alive is enabled, request-specific state is reset while the connection persists. Otherwise, the connection transitions to closing.

---

## 10. Error Handling at the Connection Level

Errors may occur at any stage of the connection lifecycle. Depending on severity and timing:

* an HTTP error response may be generated, or
* the connection may close immediately.

Once a connection enters `ERROR`, normal request processing is aborted.

---

## 11. Graceful Connection Termination

Connections terminate only through the `CLOSING` state. Cleanup is deterministic and includes:

* unregistering from the event loop,
* closing the socket,
* releasing all owned resources.

No further state transitions occur after entering `CLOSING`.


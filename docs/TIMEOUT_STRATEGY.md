# Webserv Connection Management & Adaptive Timeout Strategy

## 1. Architectural Philosophy: The "Tick-Based" Engine

In high-performance, non-blocking servers (like Nginx or Node.js), relying on system calls like `gettimeofday()` for every connection during every event loop iteration introduces massive CPU overhead. To maximize throughput and force a deeper understanding of system mechanics, this architecture replaces "wall-clock time" with a **Logical Tick System**.

### Core Principles
1. **Zero-Syscall Timing:** Time is measured in `EventLoop` cycles (ticks). The "clock" only advances when the loop iterates.
2. **Load-Adaptive Pressure:** When the server is heavily loaded, the event loop spins faster (as `epoll_wait` returns immediately with events). Consequently, ticks accumulate faster, automatically making the server more "impatient" with slow connections when resources are scarce.
3. **Decoupled Progress Trackers:** A connection's health is evaluated on two separate axes:
   - **I/O Progress (Physical):** Are bytes moving across the network?
   - **Logical Progress (Protocol):** Is the state machine advancing toward a completed HTTP request/response?

---

## 2. The Dual-Tracker System: Soft vs. Hard Limits

To successfully defend against attacks while supporting users on slow networks, every connection maintains two independent counters.

### A. The Inactivity Tracker (Soft Limit)
- **Definition:** Tracks how long the connection has been completely silent.
- **Reset Trigger:** Resets to `0` whenever **ANY** `n > 0` bytes are successfully read from or written to the socket/pipe.
- **Purpose:** Proves the client is physically connected and the network path is alive.

### B. The State Deadline (Hard Limit)
- **Definition:** Tracks how long a connection has spent in its *current logical state*.
- **Reset Trigger:** Resets to `0` ONLY when the connection transitions to a new state (e.g., `Reading_HEADERS` -> `Reading_BODY`).
- **Purpose:** Prevents "Slowloris" attacks where a malicious client drips 1 byte per minute to reset the Inactivity Tracker. The Hard Limit ensures that, regardless of network activity, a specific phase of the HTTP transaction completes within a reasonable timeframe.

---

## 3. State-by-State Progress Definitions & Thresholds

*Note: The tick values below assume a standard `epoll_wait` timeout of ~50ms during idle periods. They should be tuned based on real-world testing.*

| State | Metric of Progress | Inactivity Limit (Soft) | State Deadline (Hard) | Rationale & Security Focus |
| :--- | :--- | :--- | :--- | :--- |
| **INITIAL_CONNECTION** | Waiting for the very first byte of the request. | 50 Ticks | N/A | Defends against SYN-flood remnants and port scanners that open TCP connections but send no application data ("Ghost" connections). |
| **Reading_HEADERS** | Parser consumes bytes and moves internal cursor. | 100 Ticks | 500 Ticks | Stops Header Bloat and Slowloris. Headers must fit within `MAX_HEADER_LEN` and finish promptly. |
| **Reading_BODY** | `read()` pushes `n > 0` bytes into the input buffer. | 200 Ticks | Configurable (e.g., 2000) | Allows for large, legitimate file uploads over slow mobile networks, provided data continues to flow. |
| **CGI_EXECUTION** | `read()` pulls `n > 0` bytes from the CGI stdout pipe. | 300 Ticks | 1000 Ticks | Kills deadlocked backend scripts. A CGI process that loops infinitely without outputting data will hit this limit and be reaped. |
| **Writing_RESPONSE**| `write()` advances `sent_offset` (Kernel accepts data).| 200 Ticks | N/A | Handles slow downloaders. If the Kernel buffer is full and `write()` returns `-1` repeatedly, the client is choked or dead. |
| **IDLE (Keep-Alive)** | First byte of a *new* request is detected. | 1000+ Ticks | N/A | Keeps the socket open for subsequent requests to save TCP handshake overhead, but only if server capacity permits. |

---

## 4. Load Shedding & Resource Protection (The Panic Mode)

A robust server must defend its File Descriptors (FDs) and Memory. As the active connection count approaches the server's hard limits (e.g., `ulimit`), the server alters its patience dynamically.

| Capacity Load | Server State | Patience Strategy | Inactivity Multiplier | Accepts New Connections? |
| :--- | :--- | :--- | :--- | :--- |
| **< 70%** | **Healthy** | Relaxed | 1.0x (Standard) | **Yes.** Normal operation. |
| **70% - 90%** | **Stressed** | Moderate | 0.5x (Halved) | **Yes.** Starts aggressively pruning the slowest and IDLE clients to free up FDs. |
| **> 90%** | **Panic** | Aggressive | 0.2x (Ruthless)| **No.** `accept()` is paused or returns 503. Massive sweep of all non-essential connections. |

---

## 5. Implementation Guide: The Application Loop

To implement this without cluttering the core I/O logic, timeout checks are integrated directly into the event cycle.

### Phase 1: Action & Progress (Inside the Event Loop)
When `epoll` signals a socket is ready, or during the standard connection sweep:
1. Attempt the I/O operation (Read/Write).
2. Attempt the Logical operation (Parse/Produce).
3. If I/O succeeded $\rightarrow$ `inactivity_ticks = 0`. Else $\rightarrow$ `inactivity_ticks++`.
4. If State changed $\rightarrow$ `ticks_since_progress = 0`. Else $\rightarrow$ `ticks_since_progress++`.

### Phase 2: The Reaper
At the end of the `EventLoop` cycle, a single `sweep()` or Reaper function iterates over all connections.
1. Determine the current server Load Factor to set the `multiplier`.
2. Check `connection.inactivity_ticks > (Threshold * multiplier)`.
3. Check `connection.ticks_since_progress > State_Deadline`.
4. If either is true, mark `connection.state = Closing`.
5. Physically `close(fd)` and `delete` the connection object for all `Closing` connections safely, without disrupting the active `epoll` array.

---

## Conclusion
By shifting away from `gettimeofday()` and relying on **Derived Progress** and **Tick Constraints**, the Webserv architecture achieves production-grade resilience. It inherently protects against resource exhaustion, adapts its rules based on real-time hardware stress, and strictly enforces HTTP protocol boundaries.

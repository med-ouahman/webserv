# Timeout Strategy

## Overview

This document defines the timeout strategy for the web server, designed to prevent resource exhaustion attacks (slowloris, slow POST) and ensure efficient resource utilization while maintaining reasonable behavior for legitimate clients.

The timeout system operates on a per-connection basis with timeouts enforced at each phase of the request lifecycle.

---

## Design Constraints

- **Maximum connections**: 1000 concurrent connections
- **Enforcement mechanism**: Periodic sweep check on every event loop cycle
- **Implementation**: Single timestamp comparison per connection, O(n) sweep over all active connections
- **Scope**: HTTP request/response cycle and CGI process execution

---

## Request Lifecycle Phases

Each connection progresses through distinct phases, each with its own timeout policy:

1. **INITIAL** — Connection accepted, waiting for request line
2. **BUILDING** — Request line received, collecting headers
3. **BODY_READING** — Headers complete, reading request body (if present)
4. **PROCESSING** — Request complete, generating response (includes location resolution, file operations, response construction)
5. **WRITING** — Response being sent to client
6. **IDLE** — Response complete, connection in keep-alive state awaiting next request

---

## Timeout Values

### 1. Initial Connection Timeout
**Duration**: 5 seconds  
**Starts**: When connection is accepted via `accept()`  
**Ends**: When complete request line is received  
**Purpose**: Prevent connections that connect but never send data

**Rationale**: Legitimate HTTP clients send the request line immediately after connecting. A client that holds a connection without sending data is either broken or malicious.

**Action on timeout**: Close connection immediately without sending a response.

---

### 2. Header Timeout
**Duration**: 10 seconds  
**Starts**: When request line parsing completes  
**Ends**: When headers section completes (final `\r\n\r\n` received)  
**Purpose**: Prevent slowloris-style attacks that trickle headers slowly

**Rationale**: HTTP headers are typically small (< 8KB) and sent in a single burst. Ten seconds is generous for even slow networks.

**Action on timeout**: Send `408 Request Timeout` if possible (see error response policy), then close connection.

---

### 3. Body Progress Timeout
**Duration**: Dynamic, based on progress ratio  
**Metric**: Minimum 200 bytes/second average  
**Calculation**: `total_bytes_received / (now - body_reading_started_at) >= 200`  
**Evaluation**: Checked on every sweep cycle, but only enforced after at least 5 seconds have elapsed  
**Purpose**: Prevent slow POST attacks

**Rationale**: Even slow networks (2G mobile) can sustain 200 bytes/second. The 5-second grace period prevents false positives from bursty clients.

**Action on timeout**: Send `408 Request Timeout` if possible, then close connection.

**Special case**: If `Content-Length` is 0, body reading completes immediately; no timeout applies.

---

### 4. Processing Timeout
**Duration**: 10 seconds  
**Starts**: When request is complete (last body byte received or headers complete if no body)  
**Ends**: When response is ready to send  
**Covers**: Location resolution, routing, file system operations, autoindex generation, response construction  
**Purpose**: Prevent resource exhaustion from expensive operations

**Rationale**: Static file serving should be near-instantaneous. Autoindex generation for even large directories should complete in seconds. Exceeding this timeout indicates a server problem or attack.

**Action on timeout**: Send `500 Internal Server Error` and close connection.

**Note**: CGI execution is handled separately (see CGI Timeout section).

---

### 5. Keep-Alive Idle Timeout
**Duration**: 30 seconds  
**Starts**: When response transmission completes on a keep-alive connection  
**Ends**: When next request line begins  
**Purpose**: Reclaim resources from inactive persistent connections

**Rationale**: HTTP/1.1 persistent connections reduce latency for subsequent requests, but idle connections consume file descriptors and memory. Thirty seconds balances connection reuse benefits with resource costs.

**Action on timeout**: Close connection gracefully (no error response needed).

---

### 6. Maximum Connection Lifetime
**Duration**: 300 seconds (5 minutes)  
**Starts**: When connection is accepted  
**Ends**: Never (absolute limit)  
**Purpose**: Backstop against all timing-based attacks

**Rationale**: Even if a client carefully times requests to stay just under phase-specific timeouts, stringing together many requests or exploiting edge cases, no connection should persist indefinitely. This provides defense in depth.

**Action on timeout**: Close connection immediately, regardless of current state.

---

### 7. CGI Execution Timeout
**Duration**: 30 seconds  
**Starts**: When CGI child process is forked  
**Ends**: When CGI process exits or is killed
**Purpose**: Prevent runaway or malicious CGI scripts from consuming resources indefinitely

**Rationale**: Well-written CGI scripts execute quickly. Scripts that hang or loop infinitely must be terminated.

**Action on timeout**:
1. Send `SIGTERM` to child process
2. Wait 2 seconds for graceful exit
3. If still running, send `SIGKILL`
4. Transition connection to ERROR state
5. Send `504 Gateway Timeout` to client
6. Close connection

**Implementation note**: CGI processes are tracked in a separate vector with their own sweep mechanism. Each CGI entry must reference its owning connection (by file descriptor or connection ID) to coordinate state transitions.

---

## Implementation Details

### Timestamp Tracking

Each connection maintains three timestamps:

```cpp
struct Connection {
    time_t connection_created_at;  // When accept() completed
    time_t state_entered_at;       // When current state was entered
    time_t last_activity_at;       // When last I/O occurred
    
    // For body reading specifically:
    time_t body_reading_started_at;
    size_t body_bytes_received;
};
```

### Sweep Mechanism

```cpp
void check_timeouts() {
    time_t now = time(NULL);  // Single timestamp for entire sweep
    
    for (each connection in connection_pool) {
        check_connection_timeout(connection, now);
    }
    
    for (each cgi_process in cgi_pool) {
        check_cgi_timeout(cgi_process, now);
    }
}
```

**Frequency**: Every event loop cycle, before `epoll_wait()` or immediately after.

**Cost**: O(n) where n is the number of active connections (max 1000). With simple timestamp comparison, this is negligible compared to `epoll_wait()` overhead.

### Timeout Logic Per State

```cpp
void check_connection_timeout(Connection* conn, time_t now) {
    // Maximum lifetime check (applies to all states)
    if (now - conn->connection_created_at >= 300) {
        close_connection(conn);
        return;
    }
    
    switch (conn->state) {
        case ACCEPTED:
            // Initial timeout: 5 seconds to send request line
            if (now - conn->state_entered_at >= 5) {
                close_connection(conn);
            }
            break;
            
        case BUILDING:
            // Header timeout: 10 seconds from request line to headers complete
            if (now - conn->state_entered_at >= 10) {
                send_408_and_close(conn);
            }
            break;
            
        case BODY_READING:
            // Progress ratio: minimum 200 bytes/second, evaluated after 5 seconds
            time_t elapsed = now - conn->body_reading_started_at;
            if (elapsed >= 5) {
                double rate = (double)conn->body_bytes_received / elapsed;
                if (rate < 200.0) {
                    send_408_and_close(conn);
                }
            }
            break;
            
        case PROCESSING:
            // Processing timeout: 10 seconds to generate response
            if (now - conn->state_entered_at >= 10) {
                send_500_and_close(conn);
            }
            break;
            
        case IDLE:
            // Keep-alive timeout: 30 seconds of inactivity
            if (now - conn->last_activity_at >= 30) {
                close_connection(conn);
            }
            break;
            
        // WRITING state has no explicit timeout
        // (relies on maximum lifetime as backstop)
    }
}
```

---

## Error Response Policy

When a timeout occurs, the server may attempt to send an HTTP error response before closing the connection. However, this response transmission itself must be bounded to prevent exploitation.

### Rules

1. **Timeouts during BUILDING or BODY_READING**:
   - Attempt to send `408 Request Timeout`
   - Impose 2-second write timeout for error response transmission
   - If write doesn't complete in 2 seconds, close immediately

2. **Timeouts during PROCESSING**:
   - Attempt to send `500 Internal Server Error`
   - Same 2-second write timeout applies

3. **Timeouts during INITIAL**:
   - Close immediately without sending response
   - No HTTP communication has occurred; sending would violate protocol

4. **Timeouts during IDLE or maximum lifetime**:
   - Close gracefully without error response
   - These are not protocol errors

5. **CGI timeouts**:
   - Send `504 Gateway Timeout`
   - Same 2-second write timeout applies

### Rationale for Write Timeout on Error Responses

An attacker could trigger a timeout deliberately, then refuse to read the error response, causing the server to block on the write. The 2-second write timeout prevents this secondary attack. If the client doesn't accept the error response quickly, it confirms malicious intent, and the connection is dropped.

---

## CGI-Connection Coordination

CGI process execution occurs during the PROCESSING phase. The CGI timeout must coordinate with connection state:

### Data Structure

```cpp
struct CGIProcess {
    pid_t pid;
    int connection_fd;  // Reference to owning connection
    time_t started_at;
    time_t sigterm_sent_at;  // 0 if not sent yet
};
```

### Timeout Handling

```cpp
void check_cgi_timeout(CGIProcess* cgi, time_t now) {
    if (cgi->sigterm_sent_at == 0) {
        // First check: 30 seconds since start
        if (now - cgi->started_at >= 30) {
            kill(cgi->pid, SIGTERM);
            cgi->sigterm_sent_at = now;
        }
    } else {
        // Second check: 2 seconds since SIGTERM
        if (now - cgi->sigterm_sent_at >= 2) {
            kill(cgi->pid, SIGKILL);
            
            // Update owning connection state
            Connection* conn = find_connection_by_fd(cgi->connection_fd);
            if (conn) {
                conn->state = ERROR;
                prepare_504_response(conn);
            }
            
            remove_cgi_process(cgi);
        }
    }
}
```

### Key Points

- CGI timeout (30s) is independent of processing timeout (10s)
- In practice, CGI timeout is longer, so it's the effective bound for CGI requests
- If CGI completes before timeout, connection proceeds normally to WRITING
- If CGI times out, connection transitions to ERROR with 504 response
- Connection's processing timeout still applies if CGI completes quickly but response generation is slow

---

## Special Cases and Edge Conditions

### Pipelined Requests

HTTP/1.1 allows pipelining (client sends multiple requests without waiting for responses). Our timeout strategy handles this correctly:

- Each request resets `state_entered_at` when parsing begins
- Keep-alive timeout only applies between requests, not during pipelined burst
- Maximum connection lifetime still applies as backstop

### Partial Writes

During error response transmission, partial writes may occur. The 2-second error response write timeout should allow for multiple write attempts:

```cpp
void send_error_with_timeout(Connection* conn, int status_code) {
    time_t started = time(NULL);
    
    while (conn->write_buffer_has_data()) {
        ssize_t sent = write(conn->fd, ...);
        
        if (sent > 0) {
            advance_write_cursor(conn, sent);
        } else if (sent == -1 && errno == EAGAIN) {
            // Would block, check timeout
            if (time(NULL) - started >= 2) {
                close_connection(conn);
                return;
            }
            // Otherwise continue trying
        } else {
            // Write error
            close_connection(conn);
            return;
        }
    }
}
```

### Zero-Length Bodies

When `Content-Length: 0`, the connection immediately transitions from BUILDING to PROCESSING without entering BODY_READING. The body progress timeout does not apply.

### Missing Content-Length for POST/PUT

If a POST or PUT request lacks a `Content-Length` header, the request is malformed according to HTTP/1.1 semantics (chunked encoding is not required for this project). This should be handled as a parse error (400 Bad Request) before any body timeout applies.

### Connection During WRITING

The WRITING state has no explicit timeout. This is intentional:

- Writes are non-blocking and driven by `EPOLLOUT` readiness
- Maximum connection lifetime (300s) provides the backstop
- A slow client reading the response doesn't threaten server resources the way slow request sending does

However, if write operations consistently return EAGAIN and the connection is stuck in WRITING for an extended period approaching the maximum lifetime, this is handled by the maximum lifetime timeout.

---

## Security Guarantees

This timeout strategy provides defense against:

1. **Connection exhaustion**: Initial timeout ensures connections that never send data are quickly reclaimed
2. **Slowloris attacks**: Header timeout prevents slow header trickling
3. **Slow POST attacks**: Body progress timeout prevents slow body sending
4. **Resource exhaustion via expensive operations**: Processing timeout bounds CPU and I/O costs
5. **Indefinite connection holding**: Maximum lifetime provides absolute bound
6. **Runaway CGI scripts**: CGI timeout with forced kill
7. **Error response exploitation**: Error write timeout prevents blocking on malicious clients

---

## Performance Characteristics

- **Sweep cost**: O(n) where n ≤ 1000, negligible overhead
- **Timestamp operations**: Single `time()` call per event loop cycle
- **Memory overhead**: 3-4 timestamps (12-16 bytes) per connection
- **False positive rate**: Low, due to generous timeouts and grace periods
- **Legitimate client impact**: Minimal, all timeouts exceed normal network behavior

---

## Testing Recommendations

1. **Normal operation**: Verify legitimate requests complete without timeout
2. **Slowloris simulation**: Use `slowhttptest` or manual telnet with delayed sends
3. **Slow POST**: Send body 1 byte at a time, verify timeout at 5-second mark
4. **Keep-alive expiry**: Connect, send request, wait 31 seconds, verify closure
5. **Maximum lifetime**: Hold connection with valid requests for 301 seconds
6. **CGI timeout**: Create CGI script with infinite loop, verify SIGKILL and 504 response
7. **Error write timeout**: Trigger timeout, then refuse to read error response (e.g., via telnet without reading)

---

## Future Considerations

This timeout strategy is appropriate for the current project scope (educational, 1000 connections maximum). For production systems, consider:

- **Configurable timeouts**: Move hardcoded values to configuration
- **Per-location timeouts**: Different timeout policies for different endpoints
- **Adaptive timeouts**: Adjust based on server load or client behavior
- **Timeout metrics**: Log timeout events for security monitoring
- **Token bucket rate limiting**: Complement timeouts with connection rate limits per IP

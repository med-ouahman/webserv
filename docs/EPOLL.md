```
server_design/
│
├─ reactor_model
│   ├─ Work is proportional to events, not connections
│   ├─ Idle connections cost ~0 CPU
│   ├─ Each event cycle:
│   │     attempt bounded progress
│   │     yield back to epoll
│   └─ Never busy-loop inside event handle_request
│
├─ epoll_rules
│   ├─ Sockets must be non-blocking
│   ├─ All readiness comes from epoll
│   ├─ Do not call read/write without readiness
│   ├─ EPOLLET set once at epoll_ctl(ADD)
│   └─ epoll_ctl(MOD) only changes EPOLLIN/EPOLLOUT
│
├─ event_loop_flow
│   ├─ epoll_wait()
│   ├─ map kernel events → internal events
│   │     EPOLLIN  → READABLE
│   │     EPOLLOUT → WRITABLE
│   ├─ handle_request to Connection
│   ├─ attempt progress
│   │     drain reads
│   │     process requests
│   │     drain writes
│   └─ update epoll interest
│
├─ read_logic
│   ├─ read in loop until:
│   │     EAGAIN
│   │     parser needs more bytes
│   │     request completed
│   ├─ parser is incremental
│   └─ input buffer may accumulate partial requests
│
├─ write_logic
│   ├─ maintain:
│   │     response_buffer
│   │     sent_offset
│   ├─ write(response + offset)
│   ├─ advance offset
│   ├─ stop on:
│   │     EAGAIN
│   │     buffer drained
│   └─ partial writes are normal
│
├─ response_completion
│   ├─ if buffer drained:
│   │     if close_after_write
│   │         state = CLOSING
│   │     else
│   │         state = READING
│   └─ update epoll interest
│
├─ connection_lifecycle
│   ├─ states
│   │     READING
│   │     WRITING
│   │     CLOSING
│   └─ invariant
│         CLOSING is terminal
│         no further transitions
│
├─ connection_flags
│   └─ close_after_write
│        single authority for connection lifetime
│
├─ request_processing
│   ├─ parser.parse(buffer)
│   ├─ outcomes
│   │     CONTINUE
│   │     NEED_MORE_BYTES
│   │     PARSE_ERROR
│   │     COMPLETE
│   ├─ on COMPLETE
│   │     req = parser.get_request()
│   │     validate
│   │     handler.handle_request(req)
│   │     response_buffer = handler.serialize_current_header()
│   │     state = WRITING
│   └─ parser.reset()
│
├─ error_handling
│   ├─ malformed request
│   │     build_error_response
│   │     close_after_write = true
│   │     state = WRITING
│   └─ errors still count as requests
│
├─ request_counting
│   ├─ increment when response is generated
│   ├─ not when bytes arrive
│   ├─ not when headers parsed
│   └─ used to enforce max_requests_per_connection
│
├─ keep_alive_policy
│   ├─ HTTP/1.1 default persistent
│   ├─ close when:
│   │     Connection: close
│   │     server limit reached
│   │     protocol error
│   │     internal error
│   └─ implemented via close_after_write
│
├─ browser_compatibility
│   ├─ support HTTP/1.1
│   ├─ require Host header
│   ├─ handle favicon request
│   ├─ proper CRLF formatting
│   ├─ send Content-Length
│   └─ handle abrupt disconnects
│
├─ security_limits
│   ├─ limit requests per connection
│   ├─ limit header size
│   ├─ limit buffer growth
│   └─ avoid slow-client resource exhaustion
│
├─ epoll_interest_management
│   ├─ enable EPOLLIN when reading needed
│   ├─ enable EPOLLOUT when write buffer not empty
│   └─ disable EPOLLOUT when buffer drained
│
└─ scalability_properties
    ├─ event-driven → O(events) cost
    ├─ thousands of idle connections possible
    ├─ partial I/O handled without blocking
    └─ memory Limits are primary scaling constraint
```

```
webserver/
│
├── architecture/
│   │
│   ├── connection_role.txt
│   │   Connection translates:
│   │       bytes → HTTP (parsing)
│   │       HTTP → bytes (response)
│   │
│   ├── connection_components.txt
│   │   Connection
│   │       ├── Parser
│   │       └── HTTPDispatcher
│   │
│   └── response_trigger.txt
│       Response generation should start inside:
│           Connection::process_incoming_data()
│       when:
│           parser.parse(...) == SUCCESS
│
│
├── parser/
│   │
│   ├── responsibilities.txt
│   │   Parser handles:
│   │       - HTTP structure
│   │       - request line
│   │       - headers
│   │       - body (via Content-Length)
│   │
│   │   Parser does NOT handle:
│   │       - semantic validation
│   │       - routing
│   │       - response generation
│   │
│   ├── parser_interface.txt
│   │   enum ParseResult
│   │       NEED_MORE
│   │       SUCCESS
│   │       ERROR
│   │
│   │   Parser exposes only:
│   │       parse(bytes)
│   │       get_request_data()
│   │
│   ├── parser_state_machine.txt
│   │   REQUEST_LINE
│   │       ↓
│   │   HEADERS
│   │       ↓
│   │   HEADERS_DONE
│   │       ├── Content-Length > 0 → BODY
│   │       └── otherwise → DONE
│   │       ↓
│   │   DONE
│   │
│   └── buffer_strategy.txt
│       - parser consumes as many bytes as possible
│       - parsed bytes removed from buffer
│       - partial data → NEED_MORE
│
│
├── http_request/
│   │
│   ├── structure.txt
│   │   HTTPRequestData
│   │       method
│   │       uri
│   │       version
│   │       headers
│   │       body
│   │       body_len
│   │
│   └── design_notes.txt
│       - simple data container
│       - owned by parser
│       - reset after request processed
│
│
├── validation/
│   │
│   ├── parsing_vs_validation.txt
│   │   Parsing
│   │       checks structure only
│   │       e.g.:
│   │           METHOD SP URI SP VERSION
│   │
│   │   Validation
│   │       checks semantics
│   │       e.g.:
│   │           HTTP version valid
│   │           method supported
│   │           headers valid
│   │
│   │   Example:
│   │       GET / HTTX/1.1
│   │           structure → valid
│   │           semantics → invalid version
│   │
│   └── sanitize_request.txt
│       Connection::sanitize_request()
│           performs semantic checks
│           produces proper HTTP error codes
│
│
├── headers/
│   │
│   ├── format.txt
│   │   Header syntax:
│   │       name ":" value
│   │
│   │   value may be empty.
│   │
│   ├── mandatory_headers.txt
│   │   HTTP/1.0:
│   │       none strictly required
│   │
│   │   HTTP/1.1:
│   │       Host required
│   │
│   └── validation_rules.txt
│       - ASCII only
│       - header names case-insensitive
│       - Content-Length must be digits
│
│
├── body_parsing/
│   │
│   ├── body_detection.txt
│   │   body exists only if:
│   │       Content-Length header present
│   │
│   ├── body_rules.txt
│   │   read exactly Content-Length bytes
│   │
│   │   ignore extra bytes
│   │   (they belong to next request)
│   │
│   └── telnet_behavior.txt
│       telnet often sends:
│           body + CRLF
│
│       leftover:
│           "\r\n"
│
│       fix:
│           strip leading CRLF before next request
│
│
├── common_bugs/
│   │
│   ├── leftover_crlf.txt
│   │   problem:
│   │       next request becomes
│   │           "\r\nGET"
│   │
│   │   cause:
│   │       telnet sends CRLF after body
│   │
│   │   fix:
│   │       ignore leading empty lines
│   │
│   ├── empty_body_hang.txt
│   │   problem:
│   │       parser waits for body
│   │       even when none exists
│   │
│   │   result:
│   │       NEED_MORE forever
│   │       socket drained
│   │       connection hangs
│   │
│   │   fix:
│   │       after headers:
│   │
│   │           if Content-Length == 0
│   │               → request complete
│   │
│   └── buffer_erasing.txt
│       repeated substr() is O(n)
│
│       better:
│           track consumed bytes
│           erase once
│
│
├── utilities/
│   │
│   ├── placement.txt
│   │   stateless helpers in utils:
│   │       validate_version()
│   │       tolowercase()
│   │
│   ├── static_methods.txt
│   │   make static if:
│   │       function does not access object state
│   │
│   └── c_vs_cpp.txt
│       simple checks may use:
│           strncmp()
│
│       avoid unnecessary substr allocations
│
│
├── response/
│   │
│   ├── handler.txt
│   │   HTTPDispatcher
│   │       builds HTTP response
│   │       based on HTTPRequestData
│   │
│   └── response_object.txt
│       response structure useful for:
│           - headers
│           - body
│           - status
│
│
├── debugging/
│   │
│   ├── better_debugging.txt
│   │   debug state transitions
│   │   not just buffer contents
│   │
│   │   example:
│   │       HEADERS → BODY
│   │       BODY → DONE
│   │
│   ├── telnet_testing.txt
│   │   connect:
│   │       telnet 127.0.0.1 8080
│   │
│   │   send manual requests
│   │
│   └── deterministic_tests.txt
│       example:
│
│       printf "GET / HTTP/1.1\r\nHost: x\r\n\r\n" | nc localhost 8080
│
│
└── design_principles/
    │
    ├── parser_contract.txt
    │   parser only reports:
    │       NEED_MORE
    │       SUCCESS
    │       ERROR
    │
    ├── immutability_rule.txt
    │   data passed as const by default
    │   mutability granted explicitly
    │
    └── core_insight.txt
        non-blocking servers are:
            finite state machines
        bugs are usually:
            wrong state transitions
```

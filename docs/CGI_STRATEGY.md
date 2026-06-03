# CGI Implementation Strategy

## Document Purpose

This document outlines the complete strategy for implementing CGI (Common Gateway Interface) execution within the event-driven web server architecture. It consolidates design decisions, ownership models, state machines, error handling approaches, and critical invariants.

---

## 1. CGI Detection and Configuration

### Detection Mechanism

**Extension-Based Detection:**
- CGI execution is triggered by file extension matching
- Configuration maps extensions to interpreter paths
- Example: `.php` → `/usr/bin/php-cgi`
- Extension matching is case-insensitive
- Detection occurs during request routing phase

### Configuration Structure

**LocationCGIConfig:**
- Holds extension-to-interpreter mappings
- Stores CGI-specific settings per location
- Contains timeout configuration
- Contains working directory override

**Configuration Directives:**
- `cgi <extension> <interpreter_path>` - core mapping directive
- `cgi_timeout <seconds>` - maximum execution time
- `cgi_dir <path>` - working directory for script execution

### Validation Requirements

**Startup Validation:**
- All interpreter paths must be absolute
- All interpreters must exist on filesystem
- All interpreters must be executable
- Invalid configuration prevents server startup (fail-fast)

**Runtime Validation:**
- Script file must exist before spawning process
- Script file must be readable
- Extension must have configured interpreter
- File not found → 404, not executable → 403

---

## 2. Environment Variables

### Purpose and Mechanism

**Why Environment Variables:**
- Provide request metadata to CGI process
- Language-agnostic communication channel
- Separate metadata (environment) from content (stdin)
- Standard defined by RFC 3875

**How They're Set:**
- Built as NULL-terminated array of strings before fork
- Format: `"KEY=value"` strings
- Passed to execve as third argument
- Child process inherits complete environment snapshot

### Variable Categories

**Server Constants:**
- GATEWAY_INTERFACE - always "CGI/1.1"
- SERVER_SOFTWARE - server name and version
- SERVER_NAME - from configuration or Host header
- SERVER_PORT - listening port number

**Request Metadata:**
- REQUEST_METHOD - GET, POST, DELETE, etc.
- SERVER_PROTOCOL - HTTP version from request line
- REQUEST_URI - complete original URI
- QUERY_STRING - everything after ? in URI
- REMOTE_ADDR - client IP address
- REMOTE_PORT - client TCP port

**Script Context:**
- SCRIPT_NAME - URI path to script
- SCRIPT_FILENAME - absolute filesystem path
- PATH_INFO - extra path after script name (optional)

**Content Information:**
- CONTENT_TYPE - request body MIME type (conditional)
- CONTENT_LENGTH - exact body byte count (conditional)

**HTTP Headers Transformation:**
- All HTTP headers converted to environment variables
- Prefix with `HTTP_`
- Convert to uppercase
- Replace hyphens with underscores
- Example: `User-Agent` → `HTTP_USER_AGENT`
- Exception: Content-Type and Content-Length are NOT prefixed

### Construction Process

**Build Order:**
1. Count required variables (base + headers)
2. Allocate array with NULL terminator slot
3. Build server constants
4. Build request metadata
5. Build conditional variables (if present)
6. Transform and add HTTP headers
7. Set final element to NULL

**Memory Management:**
- Each variable string separately allocated
- Array itself separately allocated
- All must be freed after execve or on error
- Parent frees after child spawns successfully
- Environment survives in child's memory space after execve

---

## 3. CGI State Machine

### State Definitions

**CGI_SPAWNING:**
- Entry: Response handler determined CGI execution required
- Responsibilities: Fork process, create pipes, build environment, execute interpreter
- Exit: SPAWN_SUCCESS → next state, SPAWN_FAILURE → ERROR
- Resources acquired: Child process, pipe file descriptors, environment array

**CGI_WRITING_REQUEST:**
- Entry: Process spawned, request has body content
- Responsibilities: Write request body to CGI stdin, handle partial writes
- Exit: BODY_COMPLETE → CGI_READING_HEADERS, WRITE_ERROR → ERROR
- Completion action: Close stdin write end (sends EOF to CGI)

**CGI_READING_HEADERS:**
- Entry: Request body sent or no body to send
- Responsibilities: Read from stdout, buffer data, parse for header/body boundary
- Exit: HEADERS_COMPLETE → CGI_READING_BODY, PARSE_ERROR → ERROR
- Parsing target: Find "\r\n\r\n" separator, extract all headers

**CGI_READING_BODY:**
- Entry: Headers successfully parsed
- Responsibilities: Read body chunks, accumulate or stream to client
- Exit: EOF_RECEIVED or CONTENT_LENGTH_SATISFIED → CGI_COMPLETE
- Completion signals: EOF on pipe or exact Content-Length bytes read

**CGI_COMPLETE:**
- Entry: All CGI output received
- Responsibilities: Reap zombie process, cleanup resources, build HTTP response
- Exit: SUCCESS → READY_TO_WRITE
- Resource cleanup: Close pipes, free environment, kill process if needed

**CGI_TIMEOUT (Error State):**
- Entry: Execution exceeded configured timeout
- Responsibilities: Kill process forcefully, cleanup resources, build 504 error
- Exit: CLEANUP_COMPLETE → ERROR or READY_TO_WRITE
- Kill sequence: SIGTERM, wait grace period, SIGKILL if necessary

### State Machine Integration

**Connection State Flow:**
- PARSING → CGI_SPAWNING → CGI_WRITING_REQUEST → CGI_READING_HEADERS → CGI_READING_BODY → CGI_COMPLETE → READY_TO_WRITE

**State Skipping:**
- GET requests skip CGI_WRITING_REQUEST entirely
- Go directly from SPAWNING to READING_HEADERS

**Error Recovery:**
- Any CGI error → cleanup resources → generate error response → READY_TO_WRITE
- Keep-alive decision made after error response generated

---

## 4. Orchestration and Control Flow

### Responsibility Division

**Connection Responsibilities:**
- Owns client socket and manages connection lifecycle
- Enforces timeouts (connection idle and CGI execution)
- Coordinates state transitions
- Generates HTTP error responses
- Decides keep-alive behavior
- Executes actions given by handler

**Handler Responsibilities:**
- Performs request routing and path resolution
- Detects CGI requirement based on extension
- Resolves script filesystem path
- Validates file existence and permissions
- Extracts PATH_INFO from URI
<<<<<<< HEAD
- Builds CGIRequestContext with all necessary information
=======
- Builds CGIContext with all necessary information
>>>>>>> 2a4fb87 (s)
- Returns action for Connection to execute

**CGIRequestHandler Responsibilities:**
- Manages child process lifecycle
- Builds environment variable array
- Creates and manages pipes
- Writes request body to stdin
- Reads and parses CGI output
- Extracts response headers and status
- Tracks CGI-specific state and buffers

**EventPoller Responsibilities:**
- Routes epoll events to correct Connection
- Tracks global concurrent CGI count
- Enforces max concurrent CGI limit
- Does NOT own CGI state (just counts)

### Control Flow Sequence

**After CGI Detection:**
1. Handler detects CGI requirement during routing
2. Handler validates script file exists and is readable
<<<<<<< HEAD
3. Handler builds CGIRequestContext structure
4. Handler returns ResponseAction with EXECUTE_CGI type
5. Connection receives action and extracts CGIRequestContext
6. Connection allocates CGIRequestHandler instance
7. Connection passes CGIRequestContext to CGIRequestHandler.spawn()
=======
3. Handler builds CGIContext structure
4. Handler returns ResponseAction with EXECUTE_CGI type
5. Connection receives action and extracts CGIContext
6. Connection allocates CGIRequestHandler instance
7. Connection passes CGIContext to CGIRequestHandler.spawn()
>>>>>>> 2a4fb87 (s)
8. Connection transitions to CGI_SPAWNING state
9. CGIRequestHandler reports events back to Connection
10. Connection makes state transition decisions

**Handoff Interface:**
<<<<<<< HEAD
- Handler produces CGIRequestContext (routing decision)
=======
- Handler produces CGIContext (routing decision)
>>>>>>> 2a4fb87 (s)
- Connection receives ResponseAction (execution command)
- Connection delegates to CGIRequestHandler (process management)
- CGIRequestHandler reports outcomes via events
- Connection controls state machine progression

---

## 5. Ownership and Lifecycle Design

### CGI State Location

**Design Decision:**
- CGI state lives inside Connection as dynamically allocated member
- CGIRequestHandler pointer is NULL when connection not executing CGI
- Allocated only when entering CGI states
- Freed when exiting CGI states or on connection cleanup

**Structure:**
```
Connection
    ├─ client_fd
    ├─ state
    ├─ parser
    ├─ request
    └─ cgi_handler* (NULL most of the time)
```

### Ownership Semantics

**Connection Owns:**
- Client socket file descriptor
- Connection state machine
- CGIRequestHandler instance (when active)
- Request and response buffers
- Keep-alive policy

**CGIRequestHandler Owns:**
- Child process PID
- Pipe file descriptors (stdin, stdout, stderr)
- Environment variable array
- CGI output buffers
- Response parsing state

**EventPoller Tracks (but doesn't own):**
- Global count of active CGI executions
- Enforces concurrent CGI limit
- Routes events but doesn't manage CGI lifecycle

### Resource Lifecycle

**Allocation Points:**
- CGIRequestHandler allocated when entering CGI_SPAWNING
- Pipes created during spawn
- Environment array built before fork
- Output buffers allocated as needed during reading

**Cleanup Points:**
- CGI completes successfully → cleanup → free handler
- CGI times out → kill → cleanup → free handler
- CGI crashes → cleanup → free handler
- Client disconnects → kill → cleanup → free handler
- Connection destroyed → if handler exists, cleanup and free

**Cleanup Checklist:**
- Close all pipe file descriptors
- Unregister pipes from epoll
- Kill child process if still running
- Reap zombie with waitpid
- Free environment array
- Clear output buffers
- Delete CGIRequestHandler instance
- Set handler pointer to NULL

---

<<<<<<< HEAD
## 6. CGIRequestContext Structure
=======
## 6. CGIContext Structure
>>>>>>> 2a4fb87 (s)

### Purpose

Encapsulates all information needed to spawn and execute a CGI process. Produced by the handler during routing, consumed by CGIRequestHandler during spawn.

### Members

<<<<<<< HEAD
**CGIRequestContext:**
=======
**CGIContext:**
>>>>>>> 2a4fb87 (s)
- `script_filename` - absolute filesystem path to script file
- `interpreter_path` - absolute path to interpreter binary
- `script_name` - URI path to script (for SCRIPT_NAME variable)
- `path_info` - extra path component after script (for PATH_INFO variable)
- `working_directory` - directory for chdir before exec (empty means script directory)
- `timeout_seconds` - maximum execution time before kill
- `server_name` - server hostname for environment
- `server_port` - listening port for environment

### Usage Pattern

Handler builds context during routing:
- Resolves URI to filesystem path
- Looks up interpreter from extension mapping
- Extracts PATH_INFO if present
- Adds configuration values (timeout, working directory)
- Adds server identification

Connection receives context and passes to CGIRequestHandler:
- No validation needed (handler already validated)
- Directly usable for spawning
- Context is const reference (not copied)

---

## 7. Handler-Connection Communication

### Response Action Pattern

**ResponseAction Structure:**
- `action` - discriminator indicating action type
- `ready_response` - complete HTTP response (for static files, errors)
- `cgi_context` - CGI execution context (for CGI requests)

**Action Types:**
- SEND_RESPONSE - handler built complete response, just send it
- EXECUTE_CGI - handler determined CGI needed, context provided

### Design Rationale

**Separation of Concerns:**
- Handler makes routing decisions (application logic)
- Connection executes commands (I/O orchestration)
- Connection doesn't inspect request to decide what to do
- Handler doesn't know about sockets or state machines

**Type Safety:**
- Action discriminator prevents misuse of union members
- Connection must check action type before accessing data
- Accessing wrong member for action type is logic error

**Clean Interface:**
- Single return type from handler
- Connection has simple switch on action type
- No routing logic duplicated in Connection

---

## 8. Process Spawning and File Descriptors

### Fork/Exec Sequence

**Before Fork:**
- Create three pipe pairs (stdin, stdout, stderr)
- Build complete environment variable array
- Prepare argv array with interpreter and script paths
- Validate all resources ready

**After Fork (Parent):**
- Close child's pipe ends immediately
- Keep write end of stdin pipe
- Keep read ends of stdout and stderr pipes
- Register read ends with epoll (with Connection pointer as data)
- Free environment array (child has copy)
- Store child PID for later waitpid

**After Fork (Child):**
- Close parent's pipe ends immediately
- dup2 stdin pipe read end to fd 0
- dup2 stdout pipe write end to fd 1
- dup2 stderr pipe write end to fd 2
- Close original pipe fds (after duplication)
- chdir to working directory if specified
- execve with interpreter, argv, environment
- If execve fails: write error to stderr, exit non-zero

### File Descriptor Management

**Critical Invariant:**
- Unused pipe ends closed immediately after fork in both processes
- Prevents descriptor leaks
- Ensures proper EOF detection

**Close-on-Exec Flags:**
- Set FD_CLOEXEC on all server infrastructure fds before any fork
- Includes: epoll fd, listening socket, other connections' fds
- CGI pipe fds should NOT have FD_CLOEXEC (need to be inherited)
- Prevents CGI processes from inheriting unrelated descriptors

**Pipe End Lifecycle:**
- Parent stdin write end: open during WRITING_REQUEST, closed when body sent
- Parent stdout read end: open during READING states, closed when EOF received
- Parent stderr read end: open throughout, closed during cleanup
- Child's ends: closed by child after dup2, or by kernel on exec

### Exec Failure Detection

**The Problem:**
- fork succeeds but execve fails (interpreter not found, not executable)
- Parent has valid PID but child didn't become CGI process
- Need to detect this condition

**Detection Strategies:**

**Option 1 - Error Pipe:**
- Create fourth pipe specifically for exec errors
- Child writes error code to pipe only if execve fails
- Parent monitors this pipe with short timeout
- Any data received means exec failed

**Option 2 - Immediate Exit Check:**
- Child exits with non-zero status if execve fails
- Parent sets very short timeout (100ms)
- If child exits quickly with no stdout output, assume exec failure
- Check exit status from waitpid

**Option 3 - stderr Monitoring:**
- Child writes error message to stderr if execve fails
- Parent reads stderr separately
- Any stderr output before first stdout indicates problem

---

## 9. Pipe I/O and Non-Blocking Behavior

### Writing Request Body to stdin

**Edge-Triggered Requirements:**
- Never write without EPOLLOUT readiness
- Write in loop until EAGAIN received
- Track offset into request body buffer
- Resume from offset on next EPOLLOUT event

**Write Loop Pattern:**
- Calculate remaining bytes: content_length - bytes_written
- write() with remaining data
- If return > 0: advance offset, continue loop
- If return -1 and errno == EAGAIN: stop, wait for next EPOLLOUT
- If return -1 and errno == EPIPE: CGI died, transition to error

**Completion Action:**
- When bytes_written == content_length: close stdin write end
- Closing sends EOF to CGI script
- CGI knows body is complete
- Even for Content-Length: 0, must close stdin

**Special Cases:**
- GET request (no body): close stdin immediately after spawn
- POST with empty body: close stdin immediately (still sends EOF)
- Never leave stdin open without closing

### Reading CGI Output from stdout

**Edge-Triggered Requirements:**
- Read in loop until EAGAIN or EOF
- Accumulate into growing buffer during header phase
- Stream or buffer during body phase
- Never assume data arrives in one chunk

**Read Loop Pattern:**
- read() into temporary buffer
- If return > 0: append to accumulation buffer, continue loop
- If return 0: EOF detected, CGI closed stdout
- If return -1 and errno == EAGAIN: stop, wait for next EPOLLIN
- If return -1 other error: treat as CGI crash

**Header Parsing Phase:**
- Accumulate all data until "\r\n\r\n" found
- Search for boundary after each read
- If boundary found: parse everything before it as headers
- Remaining data after boundary is body (don't discard)
- Maximum header size limit (16KB) prevents memory exhaustion

**Body Reading Phase:**
- Two completion modes:
  1. Content-Length present: read exactly that many bytes
  2. No Content-Length: read until EOF
- Can stream to client incrementally or buffer completely
- Buffer overflow protection: enforce maximum body size

**EOF Handling:**
- EOF before headers complete: CGI crashed or failed
- EOF during body with Content-Length: premature termination, error
- EOF during body without Content-Length: normal completion

### Pipe Buffer Size Considerations

**Typical Pipe Buffer:**
- 64KB on modern Linux
- First write of large body might succeed for 64KB, then EAGAIN
- Must handle partial writes gracefully

**Backpressure:**
- If CGI is slow processing input, stdin pipe fills
- Parent must wait for EPOLLOUT before writing more
- Don't spin trying to write when pipe is full

**Output Buffering:**
- CGI's stdout might be line-buffered or block-buffered
- Small outputs might not appear until buffer flushes
- CGI must flush output or write enough to trigger flush
- Parent can't force CGI to flush

---

## 10. Process Lifecycle and Zombie Prevention

### Reaping Zombies

**The Zombie Problem:**
- When child exits, it becomes zombie until parent calls waitpid
- Zombie consumes PID slot and process table entry
- Multiple zombies exhaust process table

**Mandatory Invariant:**
- Every forked CGI has exactly one waitpid call
- No exceptions: success, failure, timeout, crash, disconnect
- waitpid must happen even if parent doesn't care about exit status

**When to Call waitpid:**
- When EOF detected on stdout (CGI finished output)
- After SIGKILL during timeout (forced termination)
- During error cleanup (CGI crashed)
- During connection cleanup (client disconnected)

**waitpid Flags:**
- Use WNOHANG for non-blocking check
- Returns 0 if child hasn't exited yet
- Returns PID if child exited, populates status
- Returns -1 on error

**Exit Status Interpretation:**
- WIFEXITED: child exited normally, WEXITSTATUS gives exit code
- WIFSIGNALED: child killed by signal, WTERMSIG gives signal number
- Exit code 0: success, non-zero: error
- Use to decide between 200 OK and 500 error (if no output yet)

### Timeout Enforcement

**Timeout Timer:**
- Started when entering CGI_SPAWNING state
- Checked on every event loop iteration or via timerfd
- Elapsed time calculated: current_time - spawn_time
- If elapsed > configured timeout: initiate kill sequence

**Kill Sequence:**
1. Send SIGTERM to child (polite termination request)
2. Set grace period timer (1-2 seconds)
3. Continue monitoring child
4. After grace period, check if child still alive
5. If alive: send SIGKILL (forced termination)
6. Child definitely dies after SIGKILL
7. Reap zombie with waitpid

**Process Liveness Check:**
- Use kill(pid, 0) to check if process exists
- Returns 0 if process alive, -1 with ESRCH if dead
- Don't send actual signal, just check existence

**Timeout Response:**
- Build 504 Gateway Timeout error response
- Log timeout incident with PID and elapsed time
- Cleanup all CGI resources
- Transition to READY_TO_WRITE with error response

### SIGCHLD Handling

**Option 1 - Signal Handler:**
- Install SIGCHLD handler that gets called when any child exits
- Handler must be async-signal-safe
- Complexity: coordinating with event loop
- Advantage: immediate notification of child death

**Option 2 - Polling with WNOHANG:**
- Call waitpid(pid, &status, WNOHANG) when appropriate
- When EOF detected on stdout
- During timeout checking
- During cleanup
- Advantage: simpler, no signal safety concerns

**Recommended Approach:**
- Use polling for this project
- Simpler integration with event loop
- No signal handling complexity
- Sufficient responsiveness for web server use case

---

## 11. Response Parsing and HTTP Construction

### CGI Output Format

**Structure:**
- Headers (zero or more lines)
- Blank line (CRLF CRLF)
- Body (optional)

**Header Format:**
- Standard HTTP header format: `Name: Value\r\n`
- Special header: `Status: code reason` (CGI-specific)
- No request line (unlike HTTP response format)

### Status Header Handling

**Extraction:**
- Parse all headers into key-value pairs
- Look for "Status" header (case-insensitive)
- Format: "Status: 200 OK" or "Status: 404 Not Found"
- Extract numeric code and reason phrase
- Default: 200 OK if Status header absent

**Removal:**
- Remove Status header from header list
- It's not an HTTP response header
- Used only to determine HTTP status line
- Don't send "Status: 200 OK" header to client

### HTTP Response Construction

**Status Line:**
- Build from: HTTP version (from request) + status code (from Status header or 200)
- Format: "HTTP/1.1 200 OK\r\n"

**Headers:**
- Pass through all CGI headers except Status
- Add server-generated headers:
  - Date: current timestamp
  - Server: webserv/1.0
  - Connection: close (if no keep-alive or no Content-Length from CGI)
- If Content-Length missing and body size known: add it
- Preserve header order and values exactly

**Body:**
- Append body bytes after headers
- No modification or encoding
- Body might be empty (valid for 204, redirects)

### Malformed Output Handling

**Missing Header/Body Boundary:**
- Read up to maximum header size (16KB)
- If "\r\n\r\n" not found: abort with 502 Bad Gateway
- Prevents infinite buffering from broken CGI

**Invalid Header Syntax:**
- Header without colon
- Header with invalid characters
- Abort with 502 Bad Gateway
- Log what was received for debugging

**Premature EOF:**
- EOF before headers complete: 500 Internal Server Error
- EOF during body with Content-Length: 502 Bad Gateway
- Check exit status: non-zero confirms script failure

### Content-Length Handling

**CGI Provides Content-Length:**
- Use it to know exactly how many body bytes to read
- Can support keep-alive
- Client knows response length

**CGI Omits Content-Length:**
- Read body until EOF
- Must close connection after response (Connection: close)
- Or: buffer entire body, calculate length, add header
- Or: use chunked transfer encoding (advanced)

---

## 12. Error Handling Strategy

### Error Types and HTTP Status Codes

**Routing Errors (before spawn):**
- Script not found: 404 Not Found
- Script not readable: 403 Forbidden
- Extension not mapped: 404 Not Found (or serve as static)
- Concurrent limit reached: 503 Service Unavailable

**Spawn Errors:**
- fork() failed: 500 Internal Server Error
- execve() failed: 500 Internal Server Error
- pipe() failed: 500 Internal Server Error

**Execution Errors:**
- CGI crashed (no output): 500 Internal Server Error
- CGI timeout: 504 Gateway Timeout
- Malformed output: 502 Bad Gateway
- Write to stdin failed (EPIPE): 500 Internal Server Error

**Network Errors:**
- Client disconnected: no response needed, cleanup and close
- Pipe errors: 500 Internal Server Error

### Error Response Generation

**Error Path Pattern:**
1. Detect error condition
2. Log error with details (PID, script path, error type)
3. Cleanup CGI resources completely
4. Build appropriate HTTP error response
5. Transition to READY_TO_WRITE state
6. Apply keep-alive decision

**Error Response Content:**
- Status line with correct code
- Minimal headers (Content-Type, Content-Length, Connection)
- Simple HTML body explaining error (generic, not detailed)
- Never expose internal details (paths, interpreter names)

### Resource Cleanup on Error

**Cleanup Checklist:**
- Kill child process if still running (SIGKILL)
- Close all pipe file descriptors
- Unregister all pipes from epoll
- Reap zombie with waitpid
- Free environment variable array
- Clear output buffers
- Delete CGIRequestHandler instance
- Set handler pointer to NULL

**Cleanup Ordering:**
- Kill first (if necessary)
- Then close pipes (sends signals to child if somehow still alive)
- Then reap zombie
- Then free memory
- Order matters for correctness

**Idempotent Cleanup:**
- Cleanup method can be called multiple times safely
- Check if already cleaned before performing operations
- Useful for error paths and destructor safety net

### Keep-Alive Decision After Error

**Allow Keep-Alive For:**
- Client-side errors (404, 403)
- Server errors where connection is healthy (500, 502, 504)
- HTTP/1.1 default persistent unless explicitly closed

**Force Connection Close For:**
- Partial response already sent to client
- Client disconnected during execution
- Socket-level errors
- HTTP/1.0 without Connection: keep-alive

**Implementation:**
- Set close_after_write flag based on error type
- After error response sent, check flag
- If true: transition to CLOSING
- If false: transition to READING (await next request)

---

## 13. Memory and Resource Management

### Environment Array Lifecycle

**Allocation:**
- Allocate array of char* pointers (one per variable + NULL)
- Allocate each variable string separately
- Total allocations: 1 array + N strings (N = number of variables)

**Usage:**
- Built before fork
- Passed to execve in child
- Child gets kernel-managed copy through execve

**Deallocation:**
- Parent can free immediately after successful spawn
- If spawn fails: free before returning error
- Free array and all strings it points to
- Double-free protection: set pointers to NULL after freeing

### Buffer Management

**Request Body Buffer:**
- Already exists from parsing phase
- Connection or parser owns it
- CGIRequestHandler receives const reference
- Not duplicated for CGI

**CGI Output Buffer:**
- Dynamically grows during header parsing
- Fixed size chunks during body reading
- Maximum size limit enforced (prevent DoS)
- Cleared after response built

**Response Buffer:**
- Connection owns final HTTP response
- Built from parsed CGI output
- Used during WRITING state
- Cleared after response sent (if keep-alive)

### File Descriptor Accounting

**Per-CGI Descriptors:**
- 2 pipe ends in parent (stdin write, stdout read)
- Optionally stderr read end (3rd descriptor)
- Child inherits own ends (closed after dup2)
- Total per CGI: 2-3 fds in parent process

**System Limits:**
- ulimit -n: max open files per process
- Each connection: 1 fd (client socket)
- Each CGI connection: +2-3 fds (pipes)
- Server infrastructure: 1-2 fds (epoll, listening socket)
- Must stay well under system limit

**Leak Prevention:**
- Every pipe fd creation has corresponding close
- Track fd lifecycle explicitly
- Close in cleanup paths, not just success paths
- Use RAII pattern if possible (C++ destructors)

### Concurrent Resource Limits

**Global Limits:**
- Maximum concurrent CGI processes (20-50 typical)
- Prevents fork bomb attacks
- Prevents resource exhaustion
- Enforced before allowing new CGI spawn

**Per-Connection Limits:**
- Maximum request body size (affects stdin write)
- Maximum CGI output size (affects buffer growth)
- Maximum timeout (prevents infinite execution)

**Enforcement:**
- Check global limit before transitioning to CGI_SPAWNING
- Return 503 Service Unavailable if limit reached
- Increment counter on spawn, decrement on cleanup
- Atomic operations if multi-threaded (not needed for single-thread)

---

## 14. Working Directory and Execution Context

### Working Directory Management

**Purpose:**
- CGI scripts often use relative paths
- Expect to find adjacent files (configs, data, includes)
- Need predictable working directory

**Configuration:**
- `cgi_dir` directive specifies directory
- If set: chdir to this directory before execve
- If not set: use directory containing script

**Implementation:**
- Extract directory from script_filename
- Call chdir() in child after fork, before execve
- Parent's working directory unchanged
- Only affects child process

**Error Handling:**
- If chdir() fails: log error, exit child with non-zero
- Parent detects exec failure through error pipe or quick exit
- Returns 500 Internal Server Error

### argv Array Construction

**Format:**
- argv[0]: interpreter path (by convention)
- argv[1]: script filename (absolute path)
- argv[2]: NULL (terminator)

**Example:**
- interpreter_path = "/usr/bin/php-cgi"
- script_filename = "/var/www/script.php"
- argv = ["/usr/bin/php-cgi", "/var/www/script.php", NULL]

**Why Script Path in argv:**
- Interpreter needs to know which script to execute
- Passed as command-line argument
- Some interpreters read from SCRIPT_FILENAME env instead
- Best practice: provide both (argv and environment)

### Security Considerations

**Path Validation:**
- Ensure script_filename is within allowed directories
- Prevent path traversal attacks
- Validate during routing, before spawn
- Never execute scripts outside web root

**Interpreter Whitelisting:**
- Only execute pre-configured interpreters
- Never execute arbitrary binaries
- Validate interpreter path during config load
- Fail startup if interpreter missing or not executable

**Environment Sanitization:**
- Don't include sensitive server internals
- Header values come from client (potential injection)
- CGI script must sanitize PATH_INFO and QUERY_STRING
- Don't add LD_LIBRARY_PATH or other dangerous variables

---

## 15. Edge Cases and Special Scenarios

### GET Requests (No Body)

**Optimization:**
- Skip CGI_WRITING_REQUEST state entirely
- Transition: CGI_SPAWNING → CGI_READING_HEADERS
- Close stdin immediately after spawn
- Most common case, optimize for speed

### POST with Empty Body

**Handling:**
- Content-Length: 0 present
- Still enter CGI_WRITING_REQUEST state
- Immediately close stdin (send EOF)
- Transition to next state without writing
- CGI receives EOF, knows no body coming

### CGI Produces No Output

**Valid Scenarios:**
- 204 No Content response
- Script has side effects only (database update)
- Redirect with Location header

**Detection:**
- EOF on stdout before any headers
- Check exit status: 0 = success
- Build appropriate response based on scenario

### Immediate Script Exit

**Pattern:**
- Script validates input, exits if invalid
- Very fast execution (milliseconds)
- Normal behavior, not an error

**Handling:**
- Don't assume quick exit means error
- Check exit status and any output produced
- If exit 0 and valid output: success
- If exit non-zero: error

### Client Disconnect During Execution

**Detection:**
- EPOLLHUP or EPOLLERR on client socket
- Or read() returns 0 on client socket

**Action:**
- Kill CGI immediately (SIGTERM then SIGKILL)
- No point continuing if no client to receive response
- Cleanup all resources
- Transition connection to CLOSING
- Response nowhere to send

### Multiple Concurrent CGI per Connection

**Not Supported:**
- One connection handles requests serially
- Cannot execute two CGIs simultaneously on one connection
- Previous request must complete (including CGI) before next

**Keep-Alive Behavior:**
- After CGI completes and response sent
- Connection may receive next request
- Each request gets its own CGI execution
- But never overlapping

### Large Output Streaming

**Challenge:**
- CGI produces megabytes of output
- Cannot buffer all in memory
- Must stream to client incrementally

**Strategy:**
- After headers parsed, know Content-Length
- Read chunk from CGI stdout
- Immediately write to client socket (if writable)
- Keep small buffer in flight
- Coordinate CGI read with client write events

**Complexity:**
- Connection in two I/O operations simultaneously
- Reading from CGI while writing to client
- Backpressure handling if client slow
- Advanced feature, not required for MVP

---

## 16. Testing and Debugging Strategies

### Unit Testing Targets

**Environment Building:**
- Test with different request types (GET, POST)
- Test with various headers
- Test PATH_INFO extraction
- Verify variable names and values

**Response Parsing:**
- Test with well-formed CGI output
- Test with missing Status header
- Test with empty body
- Test with malformed headers

**State Transitions:**
- Test normal flow (spawn → write → read → complete)
- Test error paths (timeout, crash, malformed)
- Test state skipping (GET skips write phase)

### Integration Testing Scenarios

**Basic Execution:**
- Simple GET to CGI that outputs "Hello World"
- Verify correct environment variables received
- Verify correct response status and headers

**POST with Body:**
- Send POST with form data
- Verify CGI receives full body via stdin
- Verify Content-Length environment variable set

**Timeout:**
- CGI script that sleeps longer than timeout
- Verify 504 error returned
- Verify process killed and reaped
- Verify no zombie processes remain

**Concurrent Requests:**
- Send multiple CGI requests simultaneously
- Verify each gets own process and pipes
- Verify no interference between requests
- Verify all processes cleaned up

**Error Conditions:**
- Request non-existent script (404)
- Request script without execute permission (403)
- CGI that exits with error code (500)
- CGI that outputs malformed headers (502)
- Client disconnect during execution

### Debugging Techniques

**State Logging:**
- Log every state transition with timestamp
- Log CGI spawn (PID, script, interpreter)
- Log when pipes created and closed
- Log when process reaped and exit status

**File Descriptor Tracking:**
- Log when pipes created (with fd numbers)
- Log when fds registered/unregistered from epoll
- Log when fds closed
- Helps catch descriptor leaks

**stderr Capture:**
- Read CGI stderr separately
- Log to server error log
- Helps debug script failures
- Never send to client (security)

**Zombie Detection:**
- Periodically check for zombie processes
- Command: `ps aux | grep defunct`
- Should always be zero
- Non-zero indicates missing waitpid

---

## 17. Critical Invariants Summary

These invariants must hold at all times. Violations indicate bugs.

### Process Management Invariants

1. **Every fork has exactly one waitpid** - no zombies ever exist
2. **Child process killed on timeout** - SIGTERM then SIGKILL if necessary  
3. **Client disconnect terminates CGI** - no orphaned processes continue
4. **Process reaped before handler deletion** - cleanup order enforced

### File Descriptor Invariants

5. **Unused pipe ends closed immediately after fork** - in both parent and child
6. **Stdin write end closed after body written** - or immediately for GET requests
7. **All pipe fds unregistered from epoll before close** - prevents events on closed fds
8. **No descriptor leaks** - every open has corresponding close in all paths

### Memory Management Invariants

9. **Every allocated environment array freed** - after execve or on error
10. **CGIRequestHandler deleted when exiting CGI states** - no memory leaks
11. **Buffers bounded** - maximum sizes enforced, prevent DoS

### State Machine Invariants

12. **Timeout timer started when CGI spawns** - not when request arrives
13. **Error paths always cleanup before transition** - resources freed first
14. **Keep-alive state reset after each request** - CGI state cleared completely
15. **One CGI per connection at a time** - never concurrent on same connection

### Data Integrity Invariants

16. **Headers parsed before trusting Content-Length** - don't allocate blindly
17. **EOF detection reliable** - pipe ends closed correctly
18. **Response status always set** - default 200 if Status header absent
19. **Client receives valid HTTP response** - even on errors

---

## 18. Performance Considerations

### Optimization Opportunities

**Fast Path for GET:**
- Skip body writing state completely
- Close stdin immediately
- Most requests are GET, optimize for this

**Environment Reuse:**
- Cannot reuse environment between requests (different headers)
- Must rebuild for each request
- Allocation overhead acceptable for CGI

**Streaming vs Buffering:**
- Buffering simpler but memory-intensive
- Streaming complex but scales better
- Choose based on use case and resources

**Concurrent Limits:**
- Balance throughput vs resource consumption
- Too low: CGI requests queue up, latency increases
- Too high: resource exhaustion, system instability
- Tune based on server capacity and CGI complexity

### Scalability Limits

**CGI is Inherently Process-Per-Request:**
- Cannot reuse processes between requests (stateless requirement)
- Fork/exec overhead on every request
- Limited by system process/memory capacity
- Not suitable for high-concurrency CGI-heavy workloads

**Mitigation Strategies:**
- Enforce concurrent CGI Limits
- Reject excess with 503 error
- Cache static assets aggressively
- Consider FastCGI for production (out of scope)

---

## 19. Summary and Key Takeaways

### Architectural Principles

**Separation of Concerns:**
- Handler resolves routes and builds context
- Connection orchestrates I/O and state machine
- CGIRequestHandler manages process and CGI protocol
- Each component has clear boundaries

**Ownership Model:**
- Connection owns CGIRequestHandler (dynamically allocated)
- CGIRequestHandler owns process and pipes
- EventPoller tracks but doesn't own
- Clear cleanup responsibility

**Event-Driven Design:**
- CGI integrated into epoll event loop
- Non-blocking I/O on all pipes
- Edge-triggered semantics
- State machine drives progress

### Implementation Priorities

**Must Have:**
- Basic GET and POST support
- Environment variable construction
- Process spawning and reaping
- Timeout enforcement
- Error handling with proper HTTP codes
- Resource cleanup on all paths

**Should Have:**
- stderr capture and logging
- Concurrent CGI limiting
- Working directory configuration
- Robust malformed output handling

**Nice to Have:**
- Output streaming to client
- Multiple interpreter support
- Detailed debugging logs
- Performance optimizations

### Common Pitfalls to Avoid

**Zombie processes** - always waitpid
**Descriptor leaks** - close unused pipe ends
**Deadlocks** - close stdin after writing
**Memory leaks** - free environment and buffers
**State corruption** - cleanup before transitions
**Timeout failures** - enforce with SIGKILL
**EOF detection** - close write ends properly
**Keep-alive bugs** - reset all CGI state

This strategy document provides the complete conceptual framework for implementing CGI support in the event-driven web server. Implementation should proceed incrementally, testing each component thoroughly before integration.

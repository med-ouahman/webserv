==============================
HTTP RESPONSE LAYER SUMMARY
==============================

ARCHITECTURE
------------

EventLoop
    handles epoll + all I/O

Connection
    owns:
        fd
        parser
        response_handler
        state
    orchestrates:
        parse → handle → send

Parser
    incremental
    states:
        REQUEST_LINE
        HEADERS
        BODY
    returns:
        NEED_MORE
        REQUEST_COMPLETE
    leftover bytes kept for next request
    reset() after request built


REQUEST → RESPONSE FLOW
-----------------------

epoll gives bytes
    ↓
Connection.recv()
    ↓
Parser.consume()
    ↓
REQUEST_COMPLETE
    ↓
ResponseHandler.handle(request)
    ↓
Response object created
    ↓
Connection switches to WRITE state
    ↓
EPOLLOUT enabled
    ↓
response streamed to client


RESPONSE TYPES
--------------

enum ResponseType
{
    SERVE_FILE,
    AUTO_INDEX,
    CGI_EXECUTE,
    STORE_FILE,
    DELETE_RESOURCE,
    REDIRECT,
    ERROR_RESPONSE,
    NO_CONTENT,
    HEADERS_ONLY
};


STREAMING DESIGN
----------------

Do NOT build entire response buffer.

Reasons:
    large files
    memory waste
    unnecessary copies

Use incremental streaming.

Connection buffer:

    char buffer[16K]


Sending model:

    Connection calls handler.advance()
    handler fills buffer with next chunk
    write() sends bytes
    repeat until finished


BODY PROVIDER ABSTRACTION
-------------------------

Purpose:
    stream body without loading into memory

Interface:

    produce(buffer, max_size)
    finished()
    has_length()
    length()

Implementations:

    FileBodyProvider
    MemoryBodyProvider
    CGIBodyProvider (future)


SENDING STRATEGY
----------------

Edge-triggered epoll rule:

    while kernel accepts bytes:
        keep writing

Typical loop:

    write()
        >0  → advance cursor
        -1 EAGAIN → stop


BUFFER STRATEGY
---------------

Connection owns fixed buffer:

    char buffer[16K]

Reasons:

    no resizing
    no allocations
    matches write() API
    cursor controls progress


CONTENT TYPE DETECTION
----------------------

Use extension → MIME map.

Examples:

    .html  → text/html
    .css   → text/css
    .js    → application/javascript
    .png   → image/png
    .jpg   → image/jpeg
    .ico   → image/x-icon
    .txt   → text/plain

Fallback:

    application/octet-stream


BROWSER BEHAVIOR
----------------

Browsers may send extra requests automatically.

Example:

    GET /favicon.ico

Clicking a tab may also create TCP connections.

Reason:

    browser connection pooling
    speculative connections


ACCEPT() BEHAVIOR
-----------------

accept() ≠ HTTP request.

accept() means:

    TCP handshake completed


Edge-triggered accept rule:

    while accept() != -1
        accept all pending connections

    stop when errno == EAGAIN


CLIENT DISCONNECT DETECTION
---------------------------

recv() results:

    >0   data received
    0    client closed connection
    -1   error

Important case:

    recv() == 0
        → client disconnected
        → close socket


EMPTY BUFFER
------------

Empty buffer does NOT mean closed connection.

Possible cases:

    no data yet
    EAGAIN
    connection idle

Only reliable signal:

    recv() == 0


COMMON IDLE CONNECTION CASE
---------------------------

Client connects but sends nothing.

Example flow:

    connect
        ↓
    accept()
        ↓
    recv() → EAGAIN

Connection still alive.


DEFENSE REQUIRED
----------------

Idle connection timeout.

Example:

    close if inactive > 10–30 seconds


IMPORTANT EPOLL RULES
---------------------

READ:

    loop recv() until EAGAIN

WRITE:

    loop write() until EAGAIN

ACCEPT:

    loop accept() until EAGAIN


KERNEL FILE BEHAVIOR
--------------------

open() does NOT load file into memory.

Kernel loads file pages lazily when read() occurs.

File can be deleted while reading because:

    open file descriptor keeps inode alive.


DESIGN PRINCIPLES
-----------------

1. EventLoop does I/O only
2. Connection orchestrates
3. Parser builds requests
4. Handler builds responses
5. BodyProvider streams data
6. Never allocate giant response buffers
7. Always stream large bodies
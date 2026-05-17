# HTTPParser — Design Document

## Table of Contents

1. [Purpose and Scope](#1-purpose-and-scope)
2. [Guiding Principle](#2-guiding-principle)
3. [Language Constraints](#3-language-constraints)
4. [Parser Contract](#4-parser-contract)
5. [Public Interface](#5-public-interface)
6. [State Machine](#6-state-machine)
7. [Line-Level Parsing Strategy](#7-line-level-parsing-strategy)
8. [Request Line](#8-request-line)
9. [Headers](#9-headers)
10. [Body Detection](#10-body-detection)
11. [Body Storage](#11-body-storage)
12. [Content-Length Body](#12-content-length-body)
13. [Chunked Transfer-Encoding Body](#13-chunked-transfer-encoding-body)
14. [Hard Limits](#14-hard-Limits)
15. [Error Handling](#15-error-handling)
16. [HTTPRequestData Structure](#16-httprequest-structure)
17. [Reset Behavior](#17-reset-behavior)
18. [Separation of Concerns](#18-separation-of-concerns)
19. [What the Parser Explicitly Rejects](#19-what-the-parser-explicitly-rejects)
20. [What the Parser Explicitly Allows](#20-what-the-parser-explicitly-allows)

---

## 1. Purpose and Scope

The HTTPParser is responsible for one thing: consuming raw bytes from a connection's read buffer and producing a complete, structurally valid `HTTPRequestData`. It is a pure HTTP structural parser. It does not perform semantic validation, routing, response generation, or any form of I/O on the connection socket.

The parser is designed to be incremental. It may receive data in arbitrarily small or large chunks — a single byte at a time or many kilobytes at once. It must handle all cases identically and produce deterministic results regardless of how the data arrives.

The parser is owned by a `Connection` object. One parser instance lives for the lifetime of one connection. It is reused across multiple requests on the same connection via `reset()`.

---

## 2. Guiding Principle

**Reject by default. Every allowance is explicit and justified.**

The parser does not attempt to be lenient, forgiving, or compatible with broken clients. If a byte sequence is not explicitly handled and permitted by the parser's design, it is rejected immediately and unconditionally with `PARSE_ERROR`.

This principle applies at every level: request line structure, header format, header values, body encoding, chunked framing, and limit enforcement. There are no grey zones, no "we'll handle it eventually" cases, and no silent tolerance of malformed input.

This posture was chosen for three reasons:

- **Security**: lenient parsers are a common source of request smuggling, header injection, and resource exhaustion vulnerabilities.
- **Correctness**: strict rejection makes the parser's behavior easy to reason about and test. Every input is either valid or an error.
- **Scope**: this is a school project. The parser covers what is needed. Anything beyond that scope is an error, not a feature to add later.

---

## 3. Language Constraints

The entire project, including the parser, is written in **C++98**. This means:

- No `constexpr` — use `static const` in namespaces for constants.
- No `enum class` — use plain `enum` with prefixed names to avoid collisions.
- No `std::unordered_map` — use `std::map` for header storage.
- No `std::string_view` — use `const std::string&` or `const char*` with length.
- No lambda functions, no range-based for loops, no `auto`.
- Non-copyable classes use the private-declaration idiom: declare copy constructor and assignment operator private and leave them undefined.

---

## 4. Parser Contract

The parser exposes a single method for consuming data:

```cpp
ParseResult parse(const char* data, std::size_t len);
```

The caller passes a pointer to the start of its read buffer and the number of bytes available. The parser advances an internal cursor through the data. After the call, the caller reads `bytes_consumed()` and erases exactly that many bytes from its buffer. This is done once per `parse()` call — not inside the parser, not mid-parse.

The parser never allocates memory from the caller's buffer. It never modifies the caller's buffer. It takes a read-only data_view of the data for the duration of the call.

`ParseResult` has exactly three values:

- `PARSE_NEED_MORE` — the data received so far is structurally valid but incomplete. The caller should wait for more bytes and call `parse()` again with the updated buffer.
- `PARSE_COMPLETE` — a full, structurally valid HTTP request has been parsed. The caller may retrieve it via `get_request_data()`.
- `PARSE_ERROR` — the data is structurally invalid and the connection should be closed after sending a 400 Bad Request response. The error is unrecoverable. No further parsing is possible on this parser instance until `reset()` is called.

There is no fourth value. There is no `PARSE_BODY_READY` pause point or storage injection mechanism. The parser handles all body I/O internally.

---

## 5. Public Interface

```cpp
class HTTPParser
{
public:
    HTTPParser(const std::string& tmp_dir, int connection_fd);
    ~HTTPParser();

    ParseResult            parse(const char* data, std::size_t len);

    std::size_t            bytes_consumed() const;
    ParseError             get_error()      const;
    const HTTPRequestData&     get_request_data()    const;  // valid after PARSE_COMPLETE

    void                   reset();
};
```

`tmp_dir` and `connection_fd` are injected at construction and used internally to derive the temp file path for body storage. The parser does not expose how or where the body is stored — that is an internal implementation detail.

`get_error()` returns a `ParseError` value. This is used internally for logging and debugging only. The caller does not use `get_error()` to decide the HTTP response — all parse errors unconditionally produce a 400 Bad Request response.

---

## 6. State Machine

### States

The state machine has the following states:

```
STATE_STRIP_LEADING_CRLF
STATE_REQUEST_LINE
STATE_HEADERS
STATE_BODY_IDENTITY
STATE_CHUNK_SIZE
STATE_CHUNK_DATA
STATE_CHUNK_TRAIL
STATE_CHUNK_LAST
STATE_DONE
STATE_ERROR
```

States represent **structural position** in the HTTP message. They answer the question: "what part of the HTTP message are we currently parsing?" They do not answer "how far through the current token are we?" — that is the job of cursors and accumulators.

Sub-states are never exposed. They are owned by the innermost scope that needs them. `scan_line()` owns line accumulation state via `line_buf_`. Body progress is tracked by `body_received_`. Chunk progress is tracked by `current_chunk_size_`. None of these appear in the `ParseState` enum.

### Transition Table

```
State                    Event                                         Next State
─────────────────────────────────────────────────────────────────────────────────
STRIP_LEADING_CRLF       CR or LF byte seen                           STRIP_LEADING_CRLF
STRIP_LEADING_CRLF       any other byte                               REQUEST_LINE

REQUEST_LINE             CRLF found, line structurally valid          HEADERS
REQUEST_LINE             CRLF found, line structurally invalid        ERROR
REQUEST_LINE             line exceeds MAX_REQUEST_LINE_LEN            ERROR
REQUEST_LINE             no CRLF yet                                  REQUEST_LINE (NEED_MORE)

HEADERS                  CRLF on non-empty line, valid header         HEADERS
HEADERS                  CRLF on non-empty line, invalid header       ERROR
HEADERS                  CRLF on empty line, no body expected         DONE
HEADERS                  CRLF on empty line, Content-Length > 0      BODY_IDENTITY
HEADERS                  CRLF on empty line, Transfer-Encoding chunk  CHUNK_SIZE
HEADERS                  header_count > MAX_HEADER_COUNT              ERROR
HEADERS                  header_block_bytes > MAX_HEADER_BLOCK_LEN    ERROR
HEADERS                  no CRLF yet                                  HEADERS (NEED_MORE)

BODY_IDENTITY            body_received == body_len                    DONE
BODY_IDENTITY            not enough bytes yet                         BODY_IDENTITY (NEED_MORE)

CHUNK_SIZE               CRLF found, valid hex size, size > 0        CHUNK_DATA
CHUNK_SIZE               CRLF found, valid hex size, size == 0       CHUNK_LAST
CHUNK_SIZE               CRLF found, invalid or extensions present    ERROR
CHUNK_SIZE               no CRLF yet                                  CHUNK_SIZE (NEED_MORE)

CHUNK_DATA               current_chunk_size == 0                         CHUNK_TRAIL
CHUNK_DATA               not enough bytes yet                         CHUNK_DATA (NEED_MORE)

CHUNK_TRAIL              CRLF found                                   CHUNK_SIZE
CHUNK_TRAIL              anything other than CRLF                     ERROR
CHUNK_TRAIL              no CRLF yet                                  CHUNK_TRAIL (NEED_MORE)

CHUNK_LAST               CRLF found (empty line)                      DONE
CHUNK_LAST               anything other than immediate CRLF           ERROR
CHUNK_LAST               no CRLF yet                                  CHUNK_LAST (NEED_MORE)

DONE                     (terminal — caller calls reset())            —
ERROR                    (terminal)                                    —
```

`STATE_ERROR` is reachable from every state. Once entered, no further transitions occur and `parse()` immediately returns `PARSE_ERROR` on any subsequent call.

---

## 7. Line-Level Parsing Strategy

The parser operates at **line level** for the request line and all headers. It does not parse individual bytes of tokens (method characters, URI characters, etc.) one at a time with separate states. Instead, it accumulates bytes into an internal string (`line_buf_`) until a complete CRLF-terminated line is found, then parses the complete line as a unit.

This is the correct granularity for HTTP. There is nothing useful the parser can do with `GET /ind` — it cannot know yet whether the method is valid, whether the URI is complete, or whether the version is present. The entire request line must be available before any of it can be interpreted.

`scan_line()` implements this:

- Reads bytes one at a time from the current parse window.
- Skips `\r` (CR) — they are consumed but not stored.
- On `\n` (LF): a complete line is ready in `line_buf_`. Returns `true`.
- On any other byte: appends to `line_buf_` after checking the line length limit.
- If the window is exhausted without finding `\n`: returns `false` — the caller returns `PARSE_NEED_MORE`.

The limit check inside `scan_line()` is per-byte — the line is rejected the moment `line_buf_.size()` exceeds the applicable limit, before the garbage has fully arrived.

Body parsing does not use `scan_line()`. Body bytes are read in bulk using `current_chunk_size_` and `body_received_` cursors against the available window.

---

## 8. Request Line

The request line has the form:

```
METHOD SP URI SP HTTP-VERSION CRLF
```

Once `scan_line()` returns a complete line, the parser validates its structure:

- Exactly two SP separators. Not tabs, not multiple spaces — exactly one SP between method and URI, and exactly one SP between URI and version.
- **Method**: non-empty sequence of uppercase alpha characters only. Maximum length `MAX_METHOD_LEN`. The parser does not validate whether the method is a known HTTP method — that is semantic validation, handled by the `Connection`.
- **URI**: must begin with `/` or `*`. No null bytes. Maximum length `MAX_URI_LEN`. The parser does not validate URI syntax beyond this — path decoding, query string parsing, and URI normalization are not the parser's responsibility.
- **Version**: must be exactly the string `HTTP/1.0` or `HTTP/1.1`. The parser checks structure here — whether the version is *supported* by the server is semantic validation handled by the `Connection`.

Any deviation from the above is `PARSE_ERROR`.

---

## 9. Headers

Each header line has the form:

```
field-name ":" field-value CRLF
```

The empty line (`CRLF` alone) terminates the header block.

For each header line produced by `scan_line()`:

- **Name**: everything before the first `:`. Must be non-empty. Must contain no spaces or tabs. Must contain only ASCII printable characters. Maximum length `MAX_HEADER_NAME_LEN`. Stored in lowercased form — header names are case-insensitive per RFC and lowercasing on store makes all lookups simple.
- **Colon**: must be present. No space before the colon — `field-name :` is rejected.
- **Value**: everything after the `:`. Leading and trailing whitespace is stripped. May be empty. Maximum length `MAX_HEADER_VALUE_LEN`.

**Duplicate headers**: any header whose name (after lowercasing) already exists in `req_.headers` is rejected with `PARSE_ERROR`. No merging, no last-wins. Every header name must be unique.

**Header folding**: a line beginning with SP or TAB is an obsolete continuation of the previous header value. This is rejected with `PARSE_ERROR`. Obsolete features are attack surface.

**Security counters**: two counters are maintained across the entire header block:

- `header_count_`: incremented per header. Checked against `MAX_HEADER_COUNT` before storing.
- `header_block_bytes_`: accumulated total of all name and value lengths. Checked against `MAX_HEADER_BLOCK_LEN` before storing.

Both checks happen before the header is stored. Violation → `PARSE_ERROR`.

**Special headers recognized during parsing** (structural relevance only):

- `content-length`: value must be all digits, no leading zeros except `"0"` itself, must fit in `size_t`, must not exceed `MAX_BODY_LEN`. Stored in `req_.body_len`.
- `transfer-encoding`: value must be exactly `"chunked"` — no other values, no comma-separated lists, no `gzip, chunked`. Multiple `transfer-encoding` headers are rejected by the duplicate header rule above.

---

## 10. Body Detection

Body detection occurs when the empty line terminating the header block is found. The parser examines the headers to determine which body mode applies:

- **Neither** `content-length` nor `transfer-encoding` present → no body, transition to `STATE_DONE`, return `PARSE_COMPLETE`.
- **`content-length` present, `transfer-encoding` absent** → transition to `STATE_BODY_IDENTITY`.
- **`transfer-encoding: chunked` present, `content-length` absent** → transition to `STATE_CHUNK_SIZE`.
- **Both present** → `PARSE_ERROR`. They are mutually exclusive. The parser does not attempt to decide which takes precedence.
- `content-length: 0` → no body, transition to `STATE_DONE` directly. The parser does not enter `STATE_BODY_IDENTITY` for a zero-length body.

---

## 11. Body Storage

The parser stores all request bodies in a temporary file on disk. There is no in-memory body storage. This decision is unconditional — it applies to all request bodies regardless of size, method, or content type.

**Justification**: the server's use cases are file uploads, CGI execution, and static file serving. In all cases either the body is large (uploads), passed to another process (CGI), or absent (GET/HEAD). There is no case where holding the body in memory provides a meaningful benefit. Disk storage avoids memory exhaustion under concurrent requests and is consistent across all request types.

The temp file path is constructed from two values injected at parser construction:

- `tmp_dir`: the configured temporary directory (e.g. `/tmp`).
- `connection_fd`: the file descriptor of the connection socket.

Path: `tmp_dir + "/body_" + connection_fd`

The parser opens this file when it first needs to write body bytes, writes incrementally as bytes arrive across multiple `parse()` calls, and closes the file descriptor when the body is complete. The path is stored in `req_.body_path`.

The parser does not delete the temp file. Lifetime management is handled by `HTTPRequestData`'s destructor.

---

## 12. Content-Length Body

In `STATE_BODY_IDENTITY`, the parser reads exactly `req_.body_len` bytes and writes them to the temp file:

- `body_received_` tracks how many bytes have been written so far.
- On each `parse()` call, the parser writes `min(body_len - body_received_, available_bytes)` bytes to the file.
- When `body_received_ == body_len`, the body is complete. The file is closed, `state_` transitions to `STATE_DONE`, and `PARSE_COMPLETE` is returned.
- Bytes beyond `Content-Length` are not consumed — `bytes_consumed()` stops exactly at the end of the body. Remaining bytes in the caller's buffer belong to the next request.

---

## 13. Chunked Transfer-Encoding Body

Chunked encoding delivers the body as a sequence of individually-sized chunks followed by a terminal zero-size chunk.

The parser uses four states to handle chunked bodies:

**`STATE_CHUNK_SIZE`**

Uses `scan_line()` to read the chunk size line. The line must contain only hexadecimal digits and nothing else. No chunk extensions (`;` and anything after it) are permitted — their presence is `PARSE_ERROR`. The hex value is parsed into `current_chunk_size_`. If the value is zero, transition to `STATE_CHUNK_LAST`. Otherwise transition to `STATE_CHUNK_DATA`. Individual chunk size must not exceed `MAX_CHUNK_SIZE`.

**`STATE_CHUNK_DATA`**

Reads exactly `current_chunk_size_` bytes and writes them to the temp file. Uses the same cursor approach as identity body — reads `min(current_chunk_size_, available)` bytes per `parse()` call, advances `current_chunk_size_`. When `current_chunk_size_` reaches zero, transitions to `STATE_CHUNK_TRAIL`.

**`STATE_CHUNK_TRAIL`**

Expects exactly `\r\n` after the chunk data. Uses `scan_line()` — the line must be empty. Any non-empty line is `PARSE_ERROR`. On success, transitions back to `STATE_CHUNK_SIZE` for the next chunk.

**`STATE_CHUNK_LAST`**

The zero-size terminal chunk has been seen. Expects exactly one empty line (`\r\n`) and nothing else. Trailing headers are not supported — any content before the final `\r\n` is `PARSE_ERROR`. On the empty line, closes the temp file, transitions to `STATE_DONE`, returns `PARSE_COMPLETE`.

The total body size across all chunks is accumulated and checked against `MAX_BODY_LEN`. If the cumulative bytes written exceed the limit, `PARSE_ERROR` is returned immediately.

---

## 14. Hard Limits

All Limits are named constants in the `ParserLimits` namespace. Exceeding any limit produces `PARSE_ERROR` immediately — not after the full value has been received.

```cpp
namespace ParserLimits
{
    static const std::size_t MAX_METHOD_LEN       = 16;
    static const std::size_t MAX_URI_LEN          = 8192;
    static const std::size_t MAX_VERSION_LEN      = 8;
    static const std::size_t MAX_REQUEST_LINE_LEN = 8230;
    static const std::size_t MAX_HEADER_NAME_LEN  = 256;
    static const std::size_t MAX_HEADER_VALUE_LEN = 8192;
    static const std::size_t MAX_HEADER_COUNT     = 50;
    static const std::size_t MAX_HEADER_BLOCK_LEN = 4096; // 4K
    static const std::size_t MAX_BODY_LEN         = 10 * 1024 * 1024;  // 10 MB // will be overridden by config::max_client_body_size
    static const std::size_t MAX_CHUNK_SIZE       = 1  * 1024 * 1024;  // 1 MB per chunk
}
```

Limit checks inside `scan_line()` are incremental — the line is rejected the moment `line_buf_.size()` would exceed the limit, before the rest of the line has arrived. A 100 MB URI is rejected after `MAX_URI_LEN` bytes, not after the full 100 MB has been read.

---

## 15. Error Handling

All errors are immediate and terminal. When any check fails, `emit_error()` is called:

```cpp
ParseResult HTTPParser::emit_error(ParseError e)
{
    error_ = e;
    state_ = STATE_ERROR;
    return PARSE_ERROR;
}
```

Once in `STATE_ERROR`, any subsequent call to `parse()` returns `PARSE_ERROR` immediately without processing any bytes.

`ParseError` values are for internal use — logging and debugging only:

```
PARSE_ERR_NONE
PARSE_ERR_LIMIT_EXCEEDED
PARSE_ERR_MALFORMED_REQUEST_LINE
PARSE_ERR_MALFORMED_HEADER
PARSE_ERR_INVALID_CONTENT_LENGTH
PARSE_ERR_INVALID_TRANSFER_ENCODING
PARSE_ERR_MALFORMED_CHUNK
PARSE_ERR_BODY_TOO_LARGE
PARSE_ERR_STORAGE_FAILURE
PARSE_ERR_UNEXPECTED_STATE
```

The `Connection` does not inspect `get_error()` when deciding the HTTP response. All `PARSE_ERROR` results map unconditionally to **400 Bad Request**. The error code is available if a logger wants to record why the request was rejected, but it has no effect on the response.

---

## 16. HTTPRequestData Structure

```cpp
struct HTTPRequestData
{
    std::string                        method;
    std::string                        uri;
    std::string                        version;     // "HTTP/1.0" or "HTTP/1.1"
    std::map<std::string, std::string> headers;     // all names lowercased
    std::string                        body_path;   // empty if no body
    std::size_t                        body_len;    // 0 if no body

    HTTPRequestData();
    ~HTTPRequestData();  // unlinks body_path if non-empty
};
```

`HTTPRequestData` is a plain data container. It owns the temp file lifetime via its destructor. When `body_path` is non-empty, the destructor calls `unlink(body_path.c_str())`. This is the only cleanup that `HTTPRequestData` performs.

The handler accesses the body by opening `body_path` when needed. It does not receive a file descriptor or a memory buffer — just a path. The handler is responsible for opening, reading, and closing the file. CGI passes the path to the child process. Nobody else holds a long-lived file descriptor to the body.

---

## 17. Reset Behavior

`Connection` calls `parser.reset()` after every request, unconditionally — regardless of whether parsing succeeded, failed mid-headers, or failed mid-body.

`parser.reset()` does the following:

- Assigns a fresh `HTTPRequestData()` to `req_`. The old instance is destroyed, triggering its destructor, which unlinks the temp file if one exists.
- Resets `state_` to `STATE_STRIP_LEADING_CRLF`.
- Resets `error_` to `PARSE_ERR_NONE`.
- Resets `pos_`, `len_`, `data_` to zero/null.
- Clears `line_buf_` (capacity retained — no reallocation).
- Resets `header_count_` and `header_block_bytes_` to zero.
- Resets `body_received_` and `current_chunk_size_` to zero.
- Closes the body file descriptor if still open.

`reset()` does not reallocate. Internal strings retain their capacity. The parser is immediately ready to receive the next request.

`HTTPRequestData` does not have a public `reset()` method. Cleanup is handled by the destructor via RAII. The connection never manually cleans up request fields — it replaces the entire instance.

---

## 18. Separation of Concerns

The parser is strictly responsible for structural parsing only. The following are explicitly outside its scope:

| Concern                                      | Owner                          |
|----------------------------------------------|-------------------------------|
| HTTP version support (is 1.0/1.1 accepted?)  | `Connection::sanitize_request()` |
| Method support (is GET/POST/DELETE valid?)    | `Connection::sanitize_request()` |
| Host header requirement for HTTP/1.1         | `Connection::sanitize_request()` |
| Content-Type interpretation                  | Handler                        |
| URI decoding and normalization               | Handler / Router               |
| Routing                                      | Router                         |
| Response generation                          | `HTTPDispatcher`          |
| Socket I/O                                   | `Connection` / `EventLoop`     |
| Keep-alive decision                          | `Connection`                   |
| Request counting                             | `Connection`                   |
| Connection timeout enforcement               | `EventLoop` / `Connection`     |

The parser proves a request is **well-formed**. The `Connection` proves it is **meaningful**. Everything downstream assumes both.

---

## 19. What the Parser Explicitly Rejects

This section enumerates every category of input the parser rejects, as a reference for testing and review.

- Request line with fewer or more than two SP separators.
- Method containing non-uppercase-alpha characters.
- Method exceeding `MAX_METHOD_LEN`.
- URI not starting with `/` or `*`.
- URI containing null bytes.
- URI exceeding `MAX_URI_LEN`.
- Version string other than exactly `HTTP/1.0` or `HTTP/1.1`.
- Request line exceeding `MAX_REQUEST_LINE_LEN`.
- Header line with no `:`.
- Header name containing spaces, tabs, or non-ASCII-printable characters.
- Header name exceeding `MAX_HEADER_NAME_LEN`.
- Space before the colon in a header (`field-name :` form).
- Header value exceeding `MAX_HEADER_VALUE_LEN`.
- Duplicate header names (case-insensitive comparison after lowercasing).
- Header folding (line starting with SP or TAB).
- More than `MAX_HEADER_COUNT` headers.
- Total header block exceeding `MAX_HEADER_BLOCK_LEN`.
- `Content-Length` value containing non-digit characters.
- `Content-Length` value with leading zeros (except `"0"` itself).
- `Content-Length` value exceeding `MAX_BODY_LEN`.
- `Transfer-Encoding` value other than exactly `"chunked"`.
- Both `Content-Length` and `Transfer-Encoding` present simultaneously.
- Chunk size line containing anything other than hex digits (no extensions, no semicolons).
- Individual chunk size exceeding `MAX_CHUNK_SIZE`.
- Non-empty content after the zero-size terminal chunk (trailing headers rejected).
- Missing or malformed `\r\n` after chunk data.
- Total body across all chunks exceeding `MAX_BODY_LEN`.
- Any input received after `STATE_ERROR` is entered.

---

## 20. What the Parser Explicitly Allows

- Leading `\r\n` sequences before the request line (telnet artifact mitigation).
- Empty header values (`field-name:` with nothing after the colon).
- `Content-Length: 0` (no body, treated as immediate completion).
- Arbitrarily fragmented delivery — any state may receive partial data and return `PARSE_NEED_MORE` an arbitrary number of times.
- Multiple `parse()` calls to complete a single line or body segment.
- Bodies written across many `parse()` calls as data arrives incrementally.
# HTTP Parser Strategy

## Goal

An HTTP parser consumes a stream of bytes and incrementally produces a
structured HTTP request representation.

The parser must support partial reads because TCP is a byte stream and
does not preserve message boundaries.

---

# Request Structure

```text
HTTPRequest {
    RequestLine
    Headers
    Body (optional)
}
```

---

# Request Grammar

```text
HTTPRequest = RequestLine HeaderSection CRLF [ MessageBody ]

RequestLine = Method SP RequestTarget SP HTTPVersion CRLF

Method = "GET"
       | "POST"
       | "DELETE"
       | "PUT"
       | "PATCH"
       | "HEAD"

RequestTarget = AbsolutePath [ "?" QueryString ]

AbsolutePath = 1*VCHAR

QueryString = *VCHAR

HTTPVersion = "HTTP/" DIGIT "." DIGIT

HeaderSection = *Header

Header = HeaderName ":" OWS HeaderValue OWS CRLF

HeaderName = 1*TokenChar

HeaderValue = *FieldChar

TokenChar = ALPHA
          | DIGIT
          | "!"
          | "#"
          | "$"
          | "%"
          | "&"
          | "'"
          | "*"
          | "+"
          | "-"
          | "."
          | "^"
          | "_"
          | "`"
          | "|"
          | "~"

FieldChar = HTAB | SP | VCHAR

MessageBody = *OCTET

DIGIT = "0".."9"

ALPHA = "A".."Z" | "a".."z"

SP = " "

HTAB = "\t"

CRLF = "\r\n"

VCHAR = %x21-7E

OCTET = %x00-FF

OWS = *(" " | "\t")
```

---

# Parsing Strategy

The parser operates as a state machine.

## States

1. REQUEST_LINE
2. HEADERS
3. BODY
4. COMPLETE
5. ERROR

---

# Parsing Rules

## Request Line

The parser reads bytes until a CRLF is encountered.

Expected format:

```text
<METHOD> <REQUEST_TARGET> <HTTP_VERSION>\r\n
```

Example:

```text
GET /index.html HTTP/1.1\r\n
```

The request line is split into exactly 3 components:

1. Method
2. Request target
3. HTTP version

Any malformed structure transitions the parser into the ERROR state.

---

## Headers

Headers are parsed line-by-line until an empty line is encountered.

Example:

```text
Host: localhost\r\n
Content-Length: 42\r\n
\r\n
```

Header format:

```text
<Name>: <Value>\r\n
```

Validation rules:

- Header names must not contain spaces
- Each header line must contain exactly one ':' separator
- Header lines terminate with CRLF
- An empty line marks the end of the header section

---

## Message Body

Body parsing depends on request headers.

Examples:

- `Content-Length`
- `Transfer-Encoding: chunked`

If no body-related headers exist, the request has no body.

The body is treated as raw bytes and should not be interpreted by the parser itself.

---

# Incremental Parsing

Because socket reads may return partial data, the parser must support:

- Partial request lines
- Partial headers
- Partial bodies

The parser should maintain internal state between calls.

Example:

```text
recv() #1:
"GET / HT"

recv() #2:
"TP/1.1\r\nHost: loca"

recv() #3:
"lhost\r\n\r\n"
```

The parser must correctly reconstruct the full request across multiple reads.

---

# Error Handling

The parser should reject:

- Invalid HTTP versions
- Malformed request lines
- Invalid header syntax
- Header overflow
- Request line overflow
- Unsupported methods
- Invalid CRLF sequences

Errors transition the parser into the ERROR state.

---

# Stateless Sub-Parsers

The HTTP parser is designed around independent stateless sub-parsers.

Each parser component is responsible for parsing a single HTTP structure:

- Request line
- Headers
- Message body

Sub-parsers must not depend on global parser state or other parsing stages.

Examples:

- The request-line parser does not know whether headers exist
- The header parser does not know whether a body exists
- The body parser does not know how headers were parsed

This separation keeps parsing logic isolated, testable, and reusable.

---

# Parser Organization

Sub-parsers are implemented as either:

- Free functions inside a namespace
- Static utility functions

They should not require ownership of the main parser object.

Example:

```text
namespace RequestLineParser
{
    ParseResult parse(const std::string& line);
}
```

The main HTTP parser acts only as:

- A state machine
- A coordinator between parsing stages
- A storage container for parsed data

---

# LineReader Utility

A `LineReader` utility is used to incrementally search for CRLF-terminated lines inside a byte stream.

The scanner abstracts partial socket reads and line reconstruction.

---

# Scan Interface

```text
ReadResult scan(size_t max_bytes);
```

The scanner searches for a CRLF sequence within the specified range.

---

# Scan Results

```text
enum ReadResult
{
    SUCCESS,
    NEED_MORE,
    LIMIT_EXCEEDED
};
```

## SUCCESS

A CRLF-terminated line was successfully found within the requested range.

## NEED_MORE

No CRLF sequence has been found yet.

The scanner expects additional bytes before the line can be completed.

## LIMIT_EXCEEDED

The maximum allowed size was reached before finding a CRLF terminator.

This protects the parser against excessively large request lines or header fields.

---

# Accessing the Line

After a successful scan, the parsed line can be retrieved using:

```text
std::string LineReader::line();
```

The returned string:

- Does not contain the trailing CRLF
- Represents a complete logical line
- Is safe for direct parser consumption

Example:

```text
Raw bytes:
"Host: localhost\r\n"

Returned line:
"Host: localhost"
```

---

# Design Rationale

The `LineReader` separates stream reconstruction from syntax parsing.

This allows sub-parsers to operate on complete logical lines instead of raw socket buffers.

Responsibilities become clearly separated:

## LineReader

Responsible for:

- Detecting CRLF
- Handling partial reads
- Enforcing maximum limits
- Producing complete lines

## Sub-Parsers

Responsible for:

- Syntax validation
- Semantic extraction
- Producing structured data

---

# Logical Request Validation

After successfully parsing the request line and headers, the parser performs a logical validation phase.

This phase validates protocol-level constraints and request consistency before body reading begins.

Logical validation occurs:

1. After parsing the request line
2. After parsing all headers
3. Before body reading starts

Although this stage belongs conceptually to the parser, it is separated from syntax parsing.

Syntax parsing answers:

```text
"Is the request structurally valid?"
```

Logical validation answers:

```text
"Is the request semantically valid according to HTTP rules?"
```

---

# Current Validation Rules

The current implementation performs minimal validation.

## Host Header Requirement

For HTTP/1.1 requests, the `Host` header is mandatory.

Example:

```text
GET / HTTP/1.1\r\n
\r\n
```

This request is invalid because the `Host` header is missing.

---

## Content-Length and Transfer-Encoding

The following headers are mutually exclusive:

- `Content-Length`
- `Transfer-Encoding`

Requests containing both are rejected.

Example:

```text
Content-Length: 42
Transfer-Encoding: chunked
```

This prevents ambiguous body framing.

---

## Transfer-Encoding Support

Currently, only the following value is supported:

```text
Transfer-Encoding: chunked
```

Any other transfer encoding is rejected with either:

- `400 Bad Request`
- `501 Not Implemented`

depending on server policy.

---

# Body Detection

After successful logical validation, the parser determines whether the request contains a body.

A body exists if one of the following headers is present:

- `Content-Length`
- `Transfer-Encoding: chunked`

However, body reading does not begin immediately.

---

# Request Resolution

Before reading the body, the request must first be resolved.

Resolution determines whether the target request logically accepts a body.

Examples:

- `POST` usually accepts a body
- `PUT` usually accepts a body
- `GET` typically ignores request bodies
- `DELETE` may ignore request bodies depending on server policy

If the resolved request does not accept a body, body reading is skipped entirely even if body-related headers are present.

This prevents unnecessary buffering and simplifies request handling.

---

# Resolution Timing

Request resolution occurs immediately after parsing the terminating CRLF sequence that ends the header section.

Example:

```text
GET / HTTP/1.1\r\n
Host: localhost\r\n
\r\n
```

The empty CRLF line marks the transition point where:

1. Header parsing ends
2. Logical validation occurs
3. Request resolution occurs
4. Body reading strategy is selected

---

# BodyReader Utility

A dedicated `BodyReader` utility handles request body streaming.

Its responsibilities include:

- Selecting body reading strategy
- Streaming body data incrementally
- Managing storage backend
- Enforcing body limits
- Handling chunked transfer decoding

---

# Body Reading Modes

The `BodyReader` supports two body framing modes:

```text
CONTENT_LENGTH
CHUNKED
```

---

# Content-Length Reading

For `Content-Length`, body reading is continuous.

The reader consumes bytes until:

```text
received_bytes == content_length
```

No additional framing logic is required.

---

# Chunked Transfer Encoding

Chunked transfer encoding is implemented as a state machine.

States:

```text
CHUNK_HEAD
CHUNK_DATA
CHUNK_TRAILER
CHUNK_LAST
```

## CHUNK_HEAD

Reads the chunk-size line.

Example:

```text
4\r\n
```

The existing `LineReader` utility is reused here to read CRLF-terminated chunk headers.

---

## CHUNK_DATA

Reads exactly the number of bytes specified by the current chunk size.

Example:

```text
Wiki
```

---

## CHUNK_TRAILER

Consumes the CRLF immediately following chunk data.

Example:

```text
\r\n
```

---

## CHUNK_LAST

Triggered when a zero-sized chunk is encountered.

Example:

```text
0\r\n
```

This marks the logical end of the body stream.

---

# Body Storage

The body reader abstracts storage behind multiple backends.

Supported storage types:

```text
BUFFER
TEMP_FILE
PERM_FILE
```

---

# BUFFER Storage

Small request bodies are stored directly in memory.

This mode is selected when:

```text
content_length <= MAX_BODY_BUFFER_SIZE
```

The body is accumulated into a string or byte buffer.

---

# TEMP_FILE Storage

Temporary disk storage is used for CGI requests.

This avoids excessive memory usage and allows efficient process streaming.

Temporary files are deleted after request completion.

---

# PERM_FILE Storage

Permanent file storage is used for upload requests.

The request body is streamed directly into the resolved destination path.

This avoids unnecessary intermediate buffering.

---

# Storage Selection

For `Content-Length`, storage selection is straightforward because the final body size is known in advance.

The reader can immediately choose between:

- memory
- temporary file
- permanent file

before body streaming begins.

---

# Chunked Storage Escalation

For chunked transfer encoding, the final body size is unknown.

The body reader therefore begins with in-memory buffering.

If the accumulated size exceeds:

```text
MAX_BODY_BUFFER_SIZE
```

storage is immediately escalated to disk.

This allows efficient handling of both:

- small chunked requests
- arbitrarily large streamed uploads

without preallocating excessive memory.

---

# Design Rationale

The parser intentionally separates:

- syntax parsing
- logical validation
- request resolution
- body streaming
- storage management

You are allowed to modifiy/ignore existing utilites if you prefer to design your own

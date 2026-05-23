
# HTTP API Design

This file describes the boundary between `http/` and the rest of `webserv`.

The event loop and connection code move bytes.

`http/` understands HTTP.

Other modules should not know about request lines, headers, CGI setup, MIME types, routing internals, or response serialization.

## Main Idea

One `http::Context` represents one request/response cycle.

It owns the HTTP state needed to receive raw request bytes, parse a `Request`, build a `Response`, and serialize response bytes.

If a connection needs multiple active requests later, the higher-level connection code can own multiple `http::Context` instances.

Responses must still be written to the client in request order.

## External Calls

The outside code talks only to `http::Context`.

Required calls:

- `ctx.consume()`
- `ctx.process()`
- `ctx.produce()`
- `ctx.state()`

Meanings:

- `consume`: receive raw bytes into HTTP.
- `process`: turn the parsed request into a response.
- `produce`: write response bytes to the communication layer.
- `state`: expose the current HTTP lifecycle state to the connection layer.

## Context Data

`Context` owns the state for one request/response cycle.

It contains:

- one communication buffer,
- one request,
- one response,
- parser progress fields,
- one small context state.

Context state should stay minimal:

- request line,
- reading headers,
- reading body,
- processing,
- response ready,
- writing response,
- done,
- error.

Do not add states unless they change external behavior.

Headers and body are separate because their timeout policies can be separate.

The connection layer maps `ContextState` to socket interest and close/reuse behavior.

The communication buffer is raw byte storage for this context.

Because one `Context` represents one request/response cycle, one communication buffer is enough inside the context.

If the higher-level connection owns multiple contexts, it is responsible for feeding request bytes into the correct context and writing completed responses in order.

## HTTP Flow

Raw bytes enter `Context`.

Parser functions operate on `Context` and turn those bytes into `Request`.

`process` resolves routing/config and chooses the correct handler function.

The handler function writes `Response`.

`produce` serializes the response into raw bytes for the communication layer.

In short:

1. Parse request bytes.
2. Build `Request`.
3. Resolve route/config.
4. Build `Response`.
5. Serialize response bytes.

## Internal Types

Classes:

- `http::Context`
- `http::Response`

Structs:

- `http::Request`

Enums:

- `http::StatusCode`
- `http::Error`
- `http::ContextState`

Parser is not a state-owning object in this design.

Parser state lives in `Context`.

The `parser/` directory contains parsing functions that operate on `Context`.

`Error` is an enum for HTTP-specific internal errors.

Use `base/` for small C++98 replacement types, especially result/optional-style return values.

Examples of where `base/` belongs:

- parser result,
- dispatch result,
- optional header lookup,
- optional route/config values.

## Pipeline

No handler objects are needed.

Use handler functions:

- `dispatch`
- `handle_file`
- `handle_directory`
- `handle_upload`
- `handle_delete`
- `handle_cgi`

`dispatch` chooses which handler function runs.

Handler functions read the request and write the response.

## Response Communication

HTTP builds the response in `Context`.

The communication layer does not inspect `Response`.

It only receives raw bytes from `produce` and sends them to the client.

## File Structure

.
├── Context.hpp
├── Error.hpp
├── StatusCode.hpp
├── Request.hpp
├── Response.hpp
├── Response.cpp
├── parser
│   ├── Parser.hpp
│   └── Parser.cpp
├── pipeline
│   ├── Dispatcher.hpp
│   ├── Dispatcher.cpp
│   └── handlers
│       ├── file.hpp
│       ├── file.cpp
│       ├── directory.hpp
│       ├── directory.cpp
│       ├── upload.hpp
│       ├── upload.cpp
│       ├── delete.hpp
│       ├── delete.cpp
│       ├── cgi.hpp
│       └── cgi.cpp
└── request_utils
    ├── request_utils.hpp
    └── request_utils.cpp

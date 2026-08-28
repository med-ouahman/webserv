# Webserv

The 42 Webserv project is an HTTP server written in C++98.

## Usage

```sh
./webserv [configuration file]
```

The project must provide a Makefile with these rules:

- `webserv` (the executable)
- `all`
- `clean`
- `fclean`
- `re`

Compilation uses `c++` with `-Wall -Wextra -Werror` and must remain compatible
with `-std=c++98`. External libraries and Boost are forbidden.

## Mandatory requirements

The server must:

- Use a configuration file supplied as an argument or from a default path.
- Stay non-blocking and handle client disconnections correctly.
- Use one `epoll()` loop for all socket and pipe I/O, including listening sockets.
- Monitor reading and writing simultaneously.
- Never read or write a socket or pipe without prior readiness notification.
- Avoid indefinite request hangs and remain operational under stress.
- Work with standard web browsers.
- Return accurate HTTP status codes and provide default error pages.
- Serve a fully static website.
- Support file uploads.
- Support `GET`, `POST`, and `DELETE`.
- Use `fork()` only for CGI execution.
- Listen on multiple ports and serve different content where configured.

Regular disk files do not need readiness polling. `select()`, `poll()`,
`epoll()`, or `kqueue()` may be used as the single event mechanism.

## Configuration file

The configuration must support:

- Interface/port pairs for one or more websites.
- Custom error pages.
- A maximum client request-body size.
- Per-route accepted HTTP methods.
- HTTP redirects.
- A filesystem root for each route.
- Directory listing enable/disable.
- Default files for directory requests.
- Upload authorization and upload storage location.
- CGI execution by file extension, including at least one CGI type.

For CGI, the request and arguments must be available through the environment,
chunked request bodies must be unchunked, EOF must be handled when no content
length is provided, and the CGI must run in the correct directory.

Provide configuration and default files that demonstrate every implemented
feature during evaluation.

## Bonus

The bonus features are:

- Cookie and session management, with simple examples.
- Support for multiple CGI types.

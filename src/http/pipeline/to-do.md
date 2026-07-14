
# CGI Integration TODO

- Resolve the script filesystem path.
- Derive `SCRIPT_NAME` from the request target.
- Derive `PATH_INFO` from the part after the matched script path.
- Derive the CGI working directory from the script path or CGI configuration.
- Build the essential CGI metadata: `REQUEST_METHOD`, `SERVER_PROTOCOL`,
  `QUERY_STRING`, `CONTENT_TYPE`, `CONTENT_LENGTH`, `GATEWAY_INTERFACE`,
  `SCRIPT_NAME`, `PATH_INFO`, `SERVER_NAME`, and `SERVER_PORT`.
- Transform applicable HTTP request headers into `HTTP_*` CGI variables.
- Pass the unresolved CGI values from routing and `Request` into `CgiHandler`.

# Report Back

Current routing already identifies CGI and stores the configured interpreter
in `DispatchInfo::cgi_path`. It also provides the normalized request path and
mapped filesystem path. It does not separately store the script name, script
path, path info, working directory, or CGI metadata.

The added `CgiHandler` expects these values through `ResolutionResult`,
`EnvBuilder`, and CGI context types that do not exist in the current HTTP API.
Its metadata list is useful, but it must be rebuilt from the current `Request`,
`DispatchInfo`, server configuration, and location configuration.

- Validate the configured CGI interpreter before execution.
- Validate the resolved CGI script path before execution.
- Decide where CGI process ownership lives: inside `CgiHandler` or in the
  higher-level event loop.

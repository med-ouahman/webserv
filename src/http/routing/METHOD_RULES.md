
# ROUTING EDGE CASES:
routing at its core is supposed to handle the following conditions:

- method support: currently supported methods are GET, POST, DELETE. - done
- path normalization is performed before location matching. - done
- the target path matches a configured location, else rejected. - done
- filesystem path is built from the selected root and normalized path. - done
- filesystem path is inspected as file, directory, executable, or not found. - done
- existing filesystem paths are checked against symlink escape. - partial
- method allowed: depending on the server config. - done
- validity of body policy with the method. - done
- the body size does not exceed server limits. - done
- redirect rules exist, if yes response can be generated before handler. - done
- CGI route match, if extension/path says CGI. - done
- upload route allowed, if method/body imply upload. - done

Already handled by the parser:
- HOST: must exist in HTTP-1.1.
- Transfer-Encoding: only accepted with chunked.
- Content-Length + Transfer-Encoding conflict: only one must exist, else rejected.

### Normalization

Implemented:
- percent-decode the request path. - done
- reject invalid percent-encoding. - done
- reject decoded NUL bytes. - done
- path_trimming: collapse repeated slashes. - done
- path_trimming: remove dot segments. - done
- path_trimming: resolve dot-dot segments. - done
- path_trimming: reject dot-dot escape above root. - done
- build filesystem path from the selected root. - done
- inspect existing filesystem path type. - done
- reject existing paths that resolve outside the root. - done

Not final:
- non-existing paths cannot be fully checked with realpath. - todo
- config location paths are not normalized before matching. - todo
- filesystem mapping still needs tests for nested locations. - todo


## HTTP Method Rules

Routing decides whether the request shape is allowed before handlers run.

### GET

- Must not have a request body.
- Content-Length: 0 is allowed.
- Content-Length greater than zero is rejected.
- Transfer-Encoding: chunked is rejected.

### POST

- May have a request body.
- Body is accepted only if the matched route/handler allows it.
- If body is accepted, HTTP reads it before final handling.
- If body is rejected, the entire request is rejected.

### DELETE

- Must not have a request body.
- Content-Length: 0 is allowed.
- Content-Length greater than zero is rejected.
- Transfer-Encoding: chunked is rejected.

### Body Policy

- If the body is accepted, HTTP reads it before final handling.
- If the body is rejected, the entire request is rejected.

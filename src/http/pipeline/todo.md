# Pipeline TODO

- DirectoryHandler: improve autoindex output.
- UploadHandler: confirm final filename policy and collision handling.
- UploadHandler: ensure upload directory exists or return the correct error.
- CGI handler: implement separately and decide how CGI headers/body are copied into `Response`.
- Keep-alive lifecycle: if response keeps connection alive, reset/create the next `Context` at connection level.
- Tests: add focused tests for each handler response status, headers, and body.

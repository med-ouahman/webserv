# Routing Duplications

- `todo.md`: verifying `DirectoryHandler` index lookup against location/server
  config is already implemented by `indexFiles()`. Remove that part of the TODO.

- `DirectoryHandler::handle()`: the null check for `decision().location` is
  redundant. Successful routing cannot produce a decision without a matched
  location.

- `RedirectHandler::handle()`: the null check for `decision().location` is
  redundant for the same reason.

- `UploadHandler::handle()`: the null check for `decision().upload_path` is
  redundant. Routing validates the upload configuration and assigns this
  pointer before returning an `UPLOAD` decision.

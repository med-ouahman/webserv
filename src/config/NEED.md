⚠ What it does not yet handle

Comments starting with # (can be added in the lexer)

Optional/unsupported directives like cgi_pass, upload_store, autoindex (easy to add in the directive parsing functions)

Some syntax validation checks beyond braces/semicolons (like multiple listen on same port, etc.)

More advanced error messages (currently generic runtime exceptions)

Subject compliance: parser now supports upload_enable/upload_path, cgi_extension/cgi_path, and size suffixes for client_max_body_size.

# Remaining Repeated Functionality

This file lists only repetition that still exists in the current source.

## Linear header lookup

Several components independently scan header collections:

- CGI request header lookup in `src/cgi/CGIContext.cpp`
- response header lookup in `src/http/Response/Response.cpp`
- request parser lookup in `src/http/Parser/headers/header_utils.cpp`

These use different header representations and case-sensitivity rules, so
they are not direct replacements. They could be unified only if the header
interfaces are redesigned around a common lookup abstraction.

#pragma once

#include <string>
#include <map>
#include "Request.hpp"

namespace http
{

/**
 * Extracts the value of a single named cookie from a raw "Cookie" header
 * value, e.g. "WEBSERV_SESSION=abc123; other=xyz".
 *
 * Returns an empty string if the header is absent or the cookie is not
 * found. Purely stateless — no dependency on HTTPRequest's concrete type,
 * so it can be reused wherever a raw header map is available.
 */

std::string extract_cookie_value(const std::vector<Header>& headers,
    const std::string& cookie_name);

} // namespace http

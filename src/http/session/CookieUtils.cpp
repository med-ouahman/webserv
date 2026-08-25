#include "CookieUtils.hpp"
#include "Request.hpp" // for std::vector<Header>
#include <iostream>

namespace http {

std::string extract_cookie_value(const std::vector<Header>& headers,
	const std::string& cookie_name) {
	// Assumes header names have already been normalized to lowercase
	// by the parser, per project convention.
	size_t cookie_index = 0;

	if (headers.empty()) return "";
	
	while (cookie_index < headers.size()) {
		if (headers[cookie_index].key == "cookie" || headers[cookie_index].key == "Cookie") {
			break;
		}

		++cookie_index;
	}

	if (cookie_index == headers.size()) return "";

	const std::string& cookie_value = headers[cookie_index].value;

	std::string target = cookie_name + "=";

	size_t pos = 0;
	while (pos < cookie_value.size()) {
		// Skip leading spaces after ';' separators
		while (pos < cookie_value.size() && cookie_value[pos] == ' ')
			++pos;

		if (cookie_value.compare(pos, target.size(), target) == 0) {
			size_t start = pos + target.size();
			size_t end = cookie_value.find(';', start);

			if (end == std::string::npos)
				return cookie_value.substr(start);
			return cookie_value.substr(start, end - start);
		}

		pos = cookie_value.find(';', pos);
		if (pos == std::string::npos)
			break;
		++pos; // skip ';'
	}

	return "";
}

} // namespace http

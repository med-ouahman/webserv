#include "CookieUtils.hpp"
#include "Request.hpp" // for std::vector<Header>
#include <iostream>

namespace http {

/*
* Policy for duplicates in cookie header
* Last wins
*/

std::string extract_cookie_value(const std::vector<Header>& headers,
	const std::string& cookie_name) {

		
	if (headers.empty()) return "";
	
	bool found = false;
	size_t cookie_index = headers.size();

	while (cookie_index > 0) {
		std::string normalized = base::toLowerCase(headers[cookie_index - 1].key);
		if ("cookie" == normalized) {
			--cookie_index;
			found = true;
			break;
		}

		--cookie_index;
	}

	if (!found) return "";

	const std::string& cookie_value = headers[cookie_index].value;

	std::string target = cookie_name + "=";

	size_t pos = 0;
	while (pos < cookie_value.size()) {
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

}

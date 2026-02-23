
#pragma once

#include <string>
#include <map>

namespace http {
	struct HTTPResponse {
		int status_code;
		std::string reason;
		std::string body;
		std::map<std::string, std::string> headers;
	};
}

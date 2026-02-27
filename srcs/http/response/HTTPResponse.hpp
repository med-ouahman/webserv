
#pragma once

#include <string>
#include <map>
#include "HTTPStatusCode.hpp"

namespace http {
	
	struct HTTPResponse {
		HTTPStatusCode status_code;
		std::string reason;
		std::string body;
		std::map<std::string, std::string> headers;
	};
}

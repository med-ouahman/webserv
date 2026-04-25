
#pragma once

#include <string>
#include <map>
#include "HTTPStatusCode.hpp"
#include "IBodyProvider.hpp"

namespace http {
	
	struct HTTPResponse {
		HTTPStatusCode status_code;
		std::string reason;
		std::string body; /* temporary body */
		IBodyProvider* body_provider;
		std::map<std::string, std::string> headers;
	};
}

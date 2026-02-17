
#pragma once 

#include <string>
#include <map>

namespace http {

	enum HTTPMethod {
		GET,
		POST,
		PATCH,
		DELETE,
		UNKNOWN
	};

	struct HTTPRequest {
		
		HTTPMethod method;
		std::string version;
		std::string url;
		std::map<std::string, std::string> headers;
		std::string body;

		HTTPMethod get_method( std::string& s ) {
			if (s == "GET")
				return GET;
			if (s == "POST")
				return POST;
			if (s == "PATCH")
				return PATCH;
			if (s == "DELETE")
				return DELETE;
			return UNKNOWN;
		}
	};
}


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
		size_t body_len;
		bool headers_done;

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
		
		HTTPRequest() {
			method = UNKNOWN;
			version.clear();
			url.clear();
			body.clear();
			headers_done = false;
			body_len = 0;
		}
	};
}

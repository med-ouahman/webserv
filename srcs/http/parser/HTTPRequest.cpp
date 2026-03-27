
#include "HTTPRequest.hpp"
#include <unistd.h>

namespace http {

    HTTPMethod HTTPRequest::get_method( std::string& s ) const {
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
	
	std::string HTTPRequest::get_method( HTTPMethod m ) const {
		if (m == GET)
			return "GET";
		if (m == POST)
			return "POST";
		if (m == PATCH)
			return "PATCH";
		if (m == DELETE)
			return "DELETE";
		return "UNKNOWN";
	}
		
	HTTPRequest::HTTPRequest() {
		method = UNKNOWN;
		version.clear();
		url.clear();
		body_path.clear();
		headers.clear();
	}

	HTTPRequest::~HTTPRequest() {
		if (body_path.size()) {
			unlink(body_path.c_str());
		}
	}

	bool HTTPRequest::want_keep_alive( void ) {
		if (version == "HTTP/1.1") {
			return headers["connection"] != "close";
		} else if (version == "HTTP/1.0") {
			return headers["connection"] == "keep-alive";
		}

		return false;
	}

	bool HTTPRequest::version_supported( void ) const {
		return version == "HTTP/1.0" || version == "HTTP/1.1";
	}


}
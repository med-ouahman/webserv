
#include "HTTPRequest.hpp"
#include <unistd.h>
#include <iostream>

namespace http {

    HTTPMethod HTTPRequest::get_method( std::string& s ) const {
		if (s == "GET")
			return GET;
		if (s == "POST")
			return POST;
		if (s == "PUT") {
			return PUT;
		}
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
		if (m == PUT)
			return "PUT";
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
		
		headers.clear();
	}

	HTTPRequest::~HTTPRequest() {
		
	}

	bool HTTPRequest::want_keep_alive() {
		if (version == "HTTP/1.1") {
			return headers["connection"] != "close";
		} else if (version == "HTTP/1.0") {
			return headers["connection"] == "keep-alive";
		}

		return false;
	}

	bool HTTPRequest::version_supported() const {
		return version == "HTTP/1.0" || version == "HTTP/1.1";
	}


}

#include "HTTPRequestData.hpp"
#include <unistd.h>
#include <iostream>

namespace http {

    HTTPMethod HTTPRequestData::get_method_name( std::string& s ) const {
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
	
	std::string HTTPRequestData::get_method_name( HTTPMethod m ) const {
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
		
	HTTPRequestData::HTTPRequestData() {
		method = UNKNOWN;
		version.clear();
		url.clear();
		
		headers.clear();
	}

	HTTPRequestData::~HTTPRequestData() {
		
	}

	bool HTTPRequestData::want_keep_alive() {
		if (version == "HTTP/1.1") {
			return headers["connection"] != "close";
		} else if (version == "HTTP/1.0") {
			return headers["connection"] == "keep-alive";
		}

		return false;
	}

	bool HTTPRequestData::version_supported() const {
		return version == "HTTP/1.0" || version == "HTTP/1.1";
	}


}
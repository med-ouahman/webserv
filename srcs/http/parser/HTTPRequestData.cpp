
#include "HTTPRequestData.hpp"
#include <unistd.h>
#include <iostream>

namespace http {

    HTTPMethod HTTPRequestData::get_method( std::string& s ) {
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
	
	std::string HTTPRequestData::get_method_name( HTTPMethod m ) {
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
		method_ = UNKNOWN;
		version_.clear();
		url_.clear();
		
		headers_.clear();
		finished_ = false;

	}

	HTTPRequestData::~HTTPRequestData() {
		
	}

	bool HTTPRequestData::want_keep_alive() {
		if (version_ == "HTTP/1.1") {
			return headers_["connection"] != "close";
		} else if (version_ == "HTTP/1.0") {
			return headers_["connection"] == "keep-alive";
		}

		return false;
	}

	bool HTTPRequestData::version_supported() const {
		return version_ == "HTTP/1.0" || version_ == "HTTP/1.1";
	}


}
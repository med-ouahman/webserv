
#include "HTTPRequest.hpp"

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
		
		HTTPRequest::HTTPRequest() {
			method = UNKNOWN;
			version.clear();
			url.clear();
			body.clear();
			headers.clear();
			headers_done = false;
			body_len = 0;
		}

		bool HTTPRequest::want_keep_alive( void ) {
			if (version == "HTTP/1.1") {
				return headers["connection"] == "keep-alive";
			} else if (version == "HTTP/1.0") {
				return headers["connection"] == "close";
			}

			return false;
		}

		bool HTTPRequest::version_supported( void ) const {
			return version == "HTTP/1.0" || version == "HTTP/1.1";
		}
}
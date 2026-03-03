
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

		HTTPMethod get_method( std::string& s ) const;
		std::string get_method( HTTPMethod m ) const;
		HTTPRequest();
		bool want_keep_alive( void );
		bool version_supported( void ) const;
	};
}

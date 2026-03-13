
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
		std::string body_path; // body is stored as a disk file.
		size_t body_len;

		HTTPMethod get_method( std::string& s ) const;
		std::string get_method( HTTPMethod m ) const;
		HTTPRequest();
		~HTTPRequest();
		bool want_keep_alive( void );
		bool version_supported( void ) const;
	};
}

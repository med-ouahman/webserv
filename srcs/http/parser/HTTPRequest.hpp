
#pragma once 

#include <string>
#include <map>

namespace http {

	enum HTTPMethod {
		GET,
		POST,
		PUT,
		PATCH,
		DELETE,
		UNKNOWN
	};

	struct HTTPRequest {
		
		HTTPMethod method;
		std::string version;
		std::string url;
		std::string query;
		std::map<std::string, std::string> headers;
		
		HTTPMethod get_method( std::string& s ) const;
		std::string get_method( HTTPMethod m ) const;
		HTTPRequest();
		~HTTPRequest();
		bool want_keep_alive();
		bool version_supported() const;
	};
}


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

	struct HTTPRequestData {
		
		HTTPMethod method;
		std::string version;
		std::string url;
		std::string uri_path;
		std::string query;
		std::map<std::string, std::string> headers;
		
		HTTPMethod get_method_name( std::string& s ) const;
		std::string get_method_name( HTTPMethod m ) const;
		HTTPRequestData();
		~HTTPRequestData();
		bool want_keep_alive();
		bool version_supported() const;
	};
}

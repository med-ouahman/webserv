
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

	struct RequestLine {
		HTTPMethod method;
		std::string version;
		std::string uri;
	};

	struct HTTPRequestData {
		
		HTTPMethod method;
		std::string version;
		std::string url;
		std::string uri_path;
		std::string query;
		std::map<std::string, std::string> headers;
		
		static HTTPMethod get_method( std::string& s );
		static std::string get_method_name( HTTPMethod m );
		HTTPRequestData();
		~HTTPRequestData();
		bool want_keep_alive();
		bool version_supported() const;
	};
}

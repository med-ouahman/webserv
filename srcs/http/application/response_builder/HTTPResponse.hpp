
#pragma once

#include <string>
#include <map>
#include "HTTPStatusCode.hpp"
#include "IBodyProvider.hpp"

namespace http {
	
	   
           class HTTPResponseType {
                public:
                    enum Type {
                        STATIC_FILE,
                        DIRECTORY,
                        CGI,
                        FILE_UPLOAD,
                        FILE_DELETE,
                        REDIRECT,
                        ERROR_RESPONSE
                    };
            };
	struct HTTPResponse {
		HTTPStatusCode status_code;
		std::string reason;
		std::string body; /* temporary body */
		IBodyProvider* body_provider;
		std::map<std::string, std::string> headers;
	};
}

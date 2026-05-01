
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

        static const char*    CRLF;
        static const ::size_t CRLF_SIZE = 2;
        static const char*    COLON;
		
        HTTPStatusCode status_code;
		std::string reason;
		std::string body;
		IBodyProvider* body_provider;
		std::map<std::string, std::string> headers;

        private:
            std::map<std::string, std::string>::iterator current_header; /* track the current header */
            std::string line_buff; /* stores incomplete headers */
        
        public:
            ::size_t serialize_headers( char* buff, ::size_t max_size );
            void serialize_current_header();
            ::ssize_t produce( char* buff, ::size_t max_size );
        public:
            enum SerializeState {
                RESPONSE_LINE,
                HEADERS,
                BODY,
                DONE
            } serialize_state;

	};
}

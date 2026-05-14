
#pragma once

#include <string>
#include <map>
#include "HTTPStatusCode.hpp"
#include "IBodyProvider.hpp"

namespace core { class BufferWriter; }

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
		std::string body; /* temporary */
		IBodyProvider* body_provider;
		std::map<std::string, std::string> headers;

        private:
            size_t serialize_headers( BufferWriter* writer );
            std::map<std::string, std::string>::iterator current_header; /* track the current header */
            std::string line_buff; /* stores incomplete headers */
        
        public:           
            ssize_t produce( BufferWriter* writer );
            
        private:
            enum SerializeState {
                RESPONSE_LINE,
                HEADERS,
                BODY,
                DONE
            } serialize_state;

	};
}

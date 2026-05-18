
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

        static const char*  CRLF;
        static const size_t CRLF_SIZE = 2;
        static const char*  COLON;
		
        HTTPStatusCode status_code;
		
		IBodyProvider* body_provider;
		std::map<std::string, std::string> headers;

        HTTPResponse();
        ~HTTPResponse();

        private:
            size_t serialize_headers( BufferWriter* writer ); /* s*/
            std::map<std::string, std::string>::iterator current_header; /* track the current header */
            std::string line_buff; /* stores incomplete headers */
            
            enum SerializeState {
                RESPONSE_LINE,
                HEADERS,
                BODY,
                DONE
            } serialize_state;
        
        public:
            ssize_t produce( BufferWriter* writer );
            void build_error_response( HTTPStatusCode code );
	};
}

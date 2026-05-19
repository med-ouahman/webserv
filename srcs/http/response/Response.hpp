
#pragma once

#include <string>
#include "StatusCode.hpp"
#include "IBodyProvider.hpp"
#include "Headers.hpp"
#include <map>

namespace http {

    class ResponseType {
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

	class Response {

        private:
            static const char*  CRLF;
            static const size_t CRLF_SIZE = 2;
            static const char*  COLON;
            StatusCode status_code;
            Headers        headers;
		    IBodyProvider* body_provider;
            size_t serialize( BufferWriter* writer );
            Headers::const_iterator current_header;
            std::string line_buff;
            
            enum SerializeState {
                RESPONSE_LINE,
                HEADERS,
                BODY,
                DONE
            } serialize_state;
        
        public:
            Response();
            ~Response();
            ssize_t produce( BufferWriter* writer );
	};
}

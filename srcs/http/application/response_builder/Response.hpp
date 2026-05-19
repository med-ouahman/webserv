
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

	struct Response {

        static const char*  CRLF;
        static const size_t CRLF_SIZE = 2;
        static const char*  COLON;
		
        StatusCode status_code;
		
		IBodyProvider* body_provider;
        Headers        headers;
        Response();
        ~Response();

        private:
            size_t serialize_headers( BufferWriter* writer ); /* s*/
            Headers::const_iterator current_header;
            std::string line_buff; /* stores incomplete headers */
            
            enum SerializeState {
                RESPONSE_LINE,
                HEADERS,
                BODY,
                DONE
            } serialize_state;
        
        public:
            ssize_t produce( BufferWriter* writer );
            void build_error_response( StatusCode code );
	};
}

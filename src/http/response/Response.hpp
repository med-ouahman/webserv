
#pragma once

#include <string>
#include "StatusCode.hpp"
#include "BodyProvider.hpp"
#include "Headers.hpp"
#include <map>

namespace http {

    enum ResponseType {
        STATIC_FILE,
        DIRECTORY,
        CGI_RESPONSE,
        FILE_UPLOAD,
        FILE_DELETE,
        REDIRECT,
        ERROR_RESPONSE
    };
    
	class Response {

        private:
            static const char*  CRLF;
            static const size_t CRLF_SIZE = 2;
            static const char*  COLON;
            
            StatusCode     status_code;
            Headers        headers;
		    BodyProvider* body_provider;

            size_t serialize( BufferWriter& writer );
            void serialize_response_line( BufferWriter& writer );
            void serialize_headers( BufferWriter& writer );
            
            Headers::const_iterator current_header;
            std::string line_buff;
            
            enum SerializeState {
                RESPONSE_LINE,
                HEADERS,
                BODY,
                DONE
            } state;
        
        public:
            Response();
            ~Response();
            ssize_t produce( BufferWriter& writer );
	};
}

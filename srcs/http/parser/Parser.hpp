#pragma once

#include <iostream>
#include "ConnectionState.hpp"
#include "ConnectionEvent.hpp"
#include "HTTPRequest.hpp"

namespace http {
    
    class Parser {
        private:
            HTTPRequest request;
            std::string request_buff;
            enum ParseState {
                REQUEST_LINE,
                HEADERS,
                BODY,
                DONE,
                ERROR

            } parse_state;
        public:
            enum ParseResult {
                SUCCESS,
                NEED_MORE_BYTES,
                PARSE_ERROR
            };
            Parser();
            ~Parser();
            ParseResult consume( const char* buff );
            HTTPRequest get_request() const;
        private:
            ParseResult parse_request_line( void );
            ParseResult parse_headers( void );
            ParseResult parse_body( void );
        private:
            static bool validate_http_version( std::string const& s );
    };
}

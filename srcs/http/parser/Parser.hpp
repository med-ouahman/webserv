#pragma once

#include <iostream>
#include "ConnectionState.hpp"
#include "ConnectionEvent.hpp"
#include "HTTPRequestData.hpp"

namespace http {
    
    class HTTPParser {
        private:
            HTTPRequestData request;
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
            HTTPParser();
            ~HTTPParser();
            ParseResult parse( char* buff );
            HTTPRequestData get_request_data() const;
        private:
            ParseResult parse_request_line();
            ParseResult parse_headers();
            ParseResult parse_body();
            bool add_request_header( std::string const& s );
        private:
            /* stateless helpers. */
            static bool validate_http_version( std::string const& s );
            
            static bool validate_headers( std::map<std::string, std::string>& headers );
    };
}

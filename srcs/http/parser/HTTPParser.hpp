#pragma once

#include <iostream>
#include "ConnectionState.hpp"
#include "ConnectionEvent.hpp"
#include "HTTPRequest.hpp"

#define BODY_MAX_LENGTH 10 * 1024 * 1024

namespace http {
    
    class HTTPParser {
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
            HTTPParser();
            ~HTTPParser();
            ParseResult consume( char* buff );
            HTTPRequest get_request() const;
            void reset( void );
        private:
            ParseResult parse_request_line( void );
            ParseResult parse_headers( void );
            ParseResult parse_body( void );
            bool add_request_header( std::string const& s );
            bool validate_headers( void );
        private:
            /* stateless helpers. */
            static bool parse_content_length( std::string const& s, size_t& body_len );
            static bool validate_http_version( std::string const& s );
            static void normalize_header_name( std::string& name );
    };
}

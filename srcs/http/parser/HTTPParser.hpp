#pragma once

#include <iostream>
#include "ConnectionState.hpp"
#include "ConnectionEvent.hpp"
#include "HTTPRequest.hpp"

#define BODY_MAX_LENGTH 10 * 1024 * 1024 // temp value (check config)

namespace http {
    
    class HTTPParser {
        private:
            const static size_t HEADERS_MAX_LENGTH = 8192;
        private:
            size_t header_bytes_parsed;
            bool headers_done;
            HTTPRequest request;
            std::string request_buff;
            size_t bytes_consumed;
        
        private:
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
                CONTINUE,
                NEED_MORE_BYTES,
                PARSE_ERROR
            };
            HTTPParser();
            ~HTTPParser();
            ParseResult consume( char* buff );
            HTTPRequest get_request() const;
            void reset( void );
        private:
            void parse_request_line( void );
            void parse_headers( void );
            void parse_body( void );
            bool add_request_header( std::string const& s );
            bool validate_headers( void );
        private:
            /* stateless helpers. */
            static bool parse_content_length( std::string const& s, size_t& body_len );
            static bool validate_http_version( std::string const& s );
            static void normalize_header_name( std::string& name );
    };
}

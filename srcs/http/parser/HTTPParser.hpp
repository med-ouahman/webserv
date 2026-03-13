#pragma once

#include <iostream>
#include "ConnectionState.hpp"
#include "ConnectionEvent.hpp"
#include "HTTPRequest.hpp"

namespace http {
    
    class HTTPParser {
        private:
            static const std::size_t MAX_METHOD_LEN       = 16;
            static const std::size_t MAX_URI_LEN          = 8192;
            static const std::size_t MAX_VERSION_LEN      = 8;
            static const std::size_t MAX_REQUEST_LINE_LEN = 8230;
            static const std::size_t MAX_HEADER_NAME_LEN  = 256;
            static const std::size_t MAX_HEADER_VALUE_LEN = 8192;
            static const std::size_t MAX_HEADER_COUNT     = 100;
            static const std::size_t MAX_HEADER_BLOCK_LEN = 16384;  // 16 KB
            static const std::size_t MAX_BODY_LEN         = 10 * 1024 * 1024;  // 10 MB
            static const std::size_t MAX_CHUNK_SIZE       = 1  * 1024 * 1024;  // 1 MB
            
        private:
            bool headers_done;
            size_t header_count;
            HTTPRequest request;
            std::string line_buff;
            size_t bytes_consumed;
            char* data_;
            size_t len_;

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
                NEED_MORE_BYTES,
                PARSE_ERROR
            };

            HTTPParser();
            ~HTTPParser();
            ParseResult consume( const char* buff, ::size_t size );
            HTTPRequest get_request() const;
            void reset( void );
        private:
            ParseResult parse_request_line( void );
            ParseResult parse_headers( void );
            ParseResult parse_body( void );
            bool add_request_header( std::string const& s );
            bool validate_headers( void );
            ParseResult scan_line( ::size_t max_bytes_allowed );

        private:
            /* stateless helpers. */
            static bool parse_content_length( std::string const& s, size_t& body_len );
            static bool validate_http_version( std::string const& s );
            static void normalize_header_name( std::string& name );
            static bool validate_header_name( const std::string& name );
    };
}

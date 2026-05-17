#pragma once

#include <iostream>
#include <stdint.h>
#include "ConnectionState.hpp"
#include "HTTPRequestData.hpp"
#include "Config.hpp"
#include "LineScanner.hpp"
#include "DataView.hpp"
#include "Expected.hpp"

namespace http {
    
    class ParseState {
        public:
            enum Type {
                REQUEST_LINE,
                HEADERS,
                BODY,
                DONE,
                ERROR
            };
    };

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
            static const std::size_t MAX_CHUNK_SIZE       = 1  * 1024 * 1024;  // 1 MB
            static const std::size_t MAX_LEADING_CRLF     = 5;
    
        private:
            bool    headers_done;
            size_t  leading_crlf_count;
            size_t  header_count;
            HTTPRequestData request;
            LineScanner line_c;
            ParseState::Type parse_state;

        public:
            class ParseResult {
                public:
                enum Type {

                    SUCCESS,
                    NEED_MORE_BYTES,
                    PARSE_ERROR,
                };
            };

            explicit HTTPParser( DataView& v );
            ~HTTPParser();
            ScanResult parse();
            HTTPRequestData& get_request_data();
            void reset();
            ParseState::Type  get_parser_state() const;
            bool finished() { return headers_done; }
            static Base::Expected<RequestLine, int> parse_request_line( const std::string& line );
            static Base::Expected<std::pair<std::string, std::string>, int> parse_headers( const std::string& line );
            
        private:
            bool add_request_header( std::string const& s );
            bool validate_headers();

        private:
            /* stateless helpers. */
            static bool validate_http_version( std::string const& s );
            static void normalize_header_name( std::string& name );
            static bool validate_header_name( const std::string& name );
    };
}

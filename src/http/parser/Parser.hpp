#pragma once

#include "Request.hpp"
#include "LineScanner.hpp"
#include "Expected.hpp"

namespace http {
    struct CGIParseContext;
    
    class Parser {
        private:
            LineScanner scanner;
        public:
            Parser();
            ~Parser();
            ScanResult parse_http_request( DataView& view );

            static Base::Expected<RequestLine, int> parse_request_line( const std::string& line );

            static Base::Expected<std::pair<std::string, std::string>, int> parse_header( const std::string& line );

            static bool validate_http_version( std::string const& s );
            static void normalize_http_header_name( std::string& name );
            static void capitalize_http_header_name( std::string& header );
            static bool validate_http_header_name( const std::string& name );
    };

    namespace http_limits {
        const std::size_t MAX_METHOD_LEN       = 16;
        const std::size_t MAX_URI_LEN          = 1024 * 2;
        const std::size_t MAX_VERSION_LEN      = 8;
        const std::size_t MAX_REQUEST_LINE_LEN = MAX_METHOD_LEN + MAX_URI_LEN + MAX_VERSION_LEN;
        const std::size_t MAX_HEADER_COUNT     = 100;
        const std::size_t MAX_HEADER_BLOCK_LEN = 1024 * 4;
        const std::size_t MAX_LEADING_CRLF     = 5;
    }
}

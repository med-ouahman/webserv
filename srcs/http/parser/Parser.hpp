#pragma once

#include "HTTPRequest.hpp"
#include "LineScanner.hpp"
#include "Expected.hpp"

namespace http {
    struct CGIParseContext;
    
    namespace parser {
            
        ScanResult parse_http_request( LineScanner& scanner, HTTPRequest& request );
      
        Base::Expected<RequestLine, int> parse_request_line( const std::string& line );
        Base::Expected<std::pair<std::string, std::string>, int> parse_header( const std::string& line );
        bool validate_http_version( std::string const& s );
        void normalize_http_header_name( std::string& name );
        void capitalize_http_header_name( std::string& header );
        bool validate_http_header_name( const std::string& name );
    }

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

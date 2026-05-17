#pragma once

#include "HTTPRequestData.hpp"
#include "LineScanner.hpp"
#include "Expected.hpp"

namespace http {

    class Parser {
        private:
            static const std::size_t MAX_METHOD_LEN       = 16;
            static const std::size_t MAX_URI_LEN          = 1024 * 2;
            static const std::size_t MAX_VERSION_LEN      = 8;
            static const std::size_t MAX_REQUEST_LINE_LEN = MAX_METHOD_LEN + MAX_URI_LEN + MAX_VERSION_LEN;
            static const std::size_t MAX_HEADER_COUNT     = 100;
            static const std::size_t MAX_HEADER_BLOCK_LEN = 1024 * 4;  // 4 KB
            static const std::size_t MAX_LEADING_CRLF     = 5;

        public:
            static ScanResult parse_http_request( LineScanner& scanner, HTTPRequestData& request );
            static ScanResult parse_cgi_output();
            static Base::Expected<RequestLine, int> parse_request_line( const std::string& line );
            static Base::Expected<std::pair<std::string, std::string>, int> parse_header( const std::string& line );
            static bool validate_http_version( std::string const& s );
            static void normalize_http_header_name( std::string& name );
            static bool validate_http_header_name( const std::string& name );
    };
}

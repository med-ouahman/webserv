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
            static const std::string hexas;

        private:
            const int conn_fd;
            const char* body_dir;
            ::size_t body_bytes_parsed;
            ::size_t body_len;
            std::string body_path;
            int body_fd;
        private:
            class ChunkState {
                public:
                    enum Type {
                        CHUNK_SIZE,
                        CHUNK_DATA,
                        CHUNK_TRAIL,
                        CHUNK_LAST
                    };
            };

            ChunkState::Type chunk_state;
            size_t chunk_remaining;

        private:
            bool headers_done;
            size_t header_count;
            HTTPRequest request;
            std::string line_buff;
            size_t bytes_consumed;
            char* data_;
            size_t len_;

        private:
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

            ParseState::Type parse_state;
            
            class BodyType {
                public:
                enum Type {
                    UNSET,
                    NONE,
                    ERROR,
                    CONTENT_LENGTH,
                    TRANSFER_ENCODING_CHUNKED,
                };
            };

            BodyType::Type body_type;

        public:
            class ParseResult {
                public:
                enum Type {

                    SUCCESS,
                    NEED_MORE_BYTES,
                    PARSE_ERROR
                };
            };

            explicit HTTPParser( int connection_fd );
            ~HTTPParser();
            ParseResult::Type consume( const char* buff, ::size_t size );
            HTTPRequest get_request() const;
            void reset( void );
            
        private:
            ParseResult::Type parse_request_line( void );
            ParseResult::Type parse_headers( void );
            ParseResult::Type parse_body( void );
            ParseResult::Type parse_body_chunked( void );
            ParseResult::Type parse_body_content_length( void );
            bool add_request_header( std::string const& s );
            bool validate_headers( void );
            ParseResult::Type scan_line( ::size_t max_bytes_allowed );
            BodyType::Type detect_body_type( void );

        private:
            /* stateless helpers. */
            static bool parse_content_length( std::string const& s, size_t& body_len );
            static bool validate_http_version( std::string const& s );
            static void normalize_header_name( std::string& name );
            static bool validate_header_name( const std::string& name );
            static ::size_t parse_chunk_size( const std::string& line_buff );
            static bool is_valid_hexa( const char c );
    };
}

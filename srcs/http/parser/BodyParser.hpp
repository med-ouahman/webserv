#pragma once

#include <stdio.h>
#include <string>
#include <map>
#include "HTTPParser.hpp"
#include "LineScanner.hpp"

namespace http {
    
    typedef HTTPParser::ParseResult ParseResult;

    struct BodyType {
        enum Type {
            UNSET,
            NONE,
            CONTENT_LENGTH,
            TRANSFER_ENCODING_CHUNKED,
            ERROR,
        };
    };

    struct ChunkState {
        
        enum Type {
            CHUNK_SIZE,
            CHUNK_DATA,
            CHUNK_TRAIL,
            CHUNK_LAST
        };
    };

	class BodyParser {

        private:
            static const std::size_t MAX_HEADER_BLOCK_LEN = 16384;
            static const std::size_t MAX_BODY_LEN         = 10 * 1024 * 1024;  // 10 MB
            static const std::size_t MAX_CHUNK_SIZE       = 1  * 1024 * 1024;
            static const std::string hexas;

		public:
			BodyParser( int fd );
			~BodyParser();
			void read_chunk( char* buff, size_t size );
			void detect_body_type( std::map<std::string, std::string>& headers );

		private:
            char* data_;
            size_t len_;
            size_t bytes_consumed;

 			const int conn_fd;
            const char* body_dir;
            size_t body_bytes_parsed;
            size_t body_len;

            std::string body_path;
            int body_fd;

            BodyType::Type body_type;
            ChunkState::Type chunk_state;
            size_t chunk_remaining;
            LineScanner sc;

        private:
            bool parse_content_length( std::string const& s );
            size_t parse_chunk_size( std::string const& line_buff );
            bool is_valid_hexa( const char c );
            ScanResult parse_body();
            ScanResult parse_body_content_length();
            ScanResult parse_body_chunked();
	};
}

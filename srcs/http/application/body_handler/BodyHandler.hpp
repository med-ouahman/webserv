#pragma once

#include <stdio.h>
#include <string>
#include <map>
#include "LineScanner.hpp"
#include <ostream>

namespace http {

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

    struct BodyState {

        enum Type {
            PREPARING,
            WRITING_BODY,
            FINISH,
            ERROR
        };
    };

    struct BodyStorage {
        enum Type {
            NONE,
            BUFFER,
            FILE_PERM,
            FILE_TEMP,
            FILE,
        };
    };

	class BodyHandler {

        private:
            static const std::size_t MAX_HEADER_BLOCK_LEN = 16384;
            static const std::size_t MAX_BODY_LEN         = 10 * 1024 * 1024;  // 10 MB /* WTF, DELETE THIS, BODY SIZE COMES FROM CONFIG!! */
            static const std::size_t MAX_CHUNK_SIZE       = 1  * 1024 * 1024;
            static const std::size_t MAX_CHUNK_EXC        = MAX_CHUNK_SIZE + 1;
            static const std::size_t MAX_BODY_BUFF_SIZE   = 1024L; // 1KB
            static const std::string hexas;

		public:
			explicit BodyHandler( int fd, core::DataView& v );
			~BodyHandler();
			ssize_t produce_body_chunk( char* buff, size_t size );
			void detect_body_type( std::map<std::string, std::string>& headers );
            ScanResult read_body( std::string const& filename );

		private:
            BodyStorage::Type body_storage;
            BodyState::Type body_state;

            core::DataView& data_view;
            std::string body_buff;
 			const int conn_fd;
            const char* body_dir;
            
            size_t body_bytes_parsed;
            size_t body_len;
            
            std::string body_path;
            int body_fd;
            
            BodyType::Type body_type;
            bool body_set;
            
            ChunkState::Type chunk_state;
            size_t chunk_remaining;
            
            LineScanner sc;

        private:
            size_t parse_chunk_size( std::string const& line_buff );
            bool is_valid_hexa( const char c );
            ScanResult read_body_content_length();
            ScanResult read_body_chunked();
            void write_body();
            void prepare_body( const std::string& filename );
            void reset();
	};
}


#include "BodyParser.hpp"
#include <sstream>
#include <fcntl.h>
#include <unistd.h>

namespace http {
    
    bool BodyParser::is_valid_hexa( const char c ) {
		return hexas.find(c) != std::string::npos;
	}
    
   ScanResult BodyParser::parse_body() {

        if (BodyType::UNSET == body_type) {

            if (body_type == BodyType::ERROR) {
                return ERROR;
            } else if (body_type == BodyType::NONE) {
                return SUCCESS;
            }

            std::stringstream ss;
            ss << conn_fd;
            body_path = body_dir + std::string("/tmp_body_") + ss.str();
            body_fd = open(body_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);
            if (body_fd < 0) {
                return ERROR;
            }
        }

        ScanResult result;
        switch (body_type) {
            case BodyType::CONTENT_LENGTH:
                result = parse_body_content_length();
                break;
            case BodyType::TRANSFER_ENCODING_CHUNKED:
                result = parse_body_chunked();
                break;
            default:
                result = NEED_MORE;
        }

        if (result == SUCCESS) {
            ::close(body_fd);
            body_fd = -1;
        }
    
        return result;
    }
    
    ScanResult BodyParser::parse_body_chunked() {

        if (chunk_state == ChunkState::CHUNK_SIZE) {

            ScanResult r = sc.scan(MAX_HEADER_BLOCK_LEN);

            if (r != SUCCESS) {
                return r;
            }
            
            chunk_remaining = parse_chunk_size(sc.line());
            if (chunk_remaining > MAX_CHUNK_SIZE) {
                return ERROR;
            }
            
            sc.reset();
            
            if (chunk_remaining == 0) {
                chunk_state = ChunkState::CHUNK_LAST;
            } else {
                chunk_state = ChunkState::CHUNK_DATA;
            }
        }

        if (chunk_state == ChunkState::CHUNK_LAST) {
            ScanResult res = sc.scan(MAX_HEADER_BLOCK_LEN);
            if (res != SUCCESS) {
                return res;
            }
            if (sc.line().size() != 0) {
                return ERROR;
            }

            return SUCCESS;
        }

        ::size_t remaining = chunk_remaining - body_bytes_parsed;
        ::size_t to_copy = std::min(remaining, len_ - bytes_consumed);
        ::write(body_fd, data_ + bytes_consumed, to_copy);
        body_bytes_parsed += to_copy;
        bytes_consumed += to_copy;
        
        if (body_bytes_parsed == chunk_remaining) {
            body_bytes_parsed = 0;
            sc.reset();
            chunk_state = ChunkState::CHUNK_SIZE;
        }

        return NEED_MORE;
    }

    ScanResult BodyParser::parse_body_content_length() {
    
        ::size_t remaining = body_len - body_bytes_parsed;
        ::size_t available = len_ - bytes_consumed;
        
        ::size_t to_copy = std::min(remaining, available);

        ::write(body_fd, data_ + bytes_consumed, to_copy);
        
        body_bytes_parsed += to_copy;
        bytes_consumed += to_copy;

        if (body_bytes_parsed == body_len) {
            return SUCCESS;
        }
        
        return NEED_MORE;
    }
}

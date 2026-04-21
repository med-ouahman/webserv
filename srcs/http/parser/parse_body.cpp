
#include "HTTPParser.hpp"
#include <sstream>
#include <fcntl.h>
#include <unistd.h>

namespace http {

    HTTPParser::ParseResult::Type HTTPParser::parse_body( void ) {

        if (BodyType::UNSET == body_type) {

            body_type = detect_body_type();

            if (body_type == BodyType::ERROR) {
                return ParseResult::PARSE_ERROR;
            } else if (body_type == BodyType::NONE) {
                return ParseResult::SUCCESS;
            }

            std::stringstream ss;
            ss << conn_fd;
            body_path = body_dir + std::string("/tmp_body_") + ss.str();
            body_fd = open(body_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);
            if (body_fd < 0) {
                return ParseResult::PARSE_ERROR;
            }
        }

        ParseResult::Type result;
        switch (body_type) {
            case BodyType::CONTENT_LENGTH:
                result = parse_body_content_length();
                break;
            case BodyType::TRANSFER_ENCODING_CHUNKED:
                result = parse_body_chunked();
                break;
            default:
                result = ParseResult::NEED_MORE_BYTES;
        }

        if (result == ParseResult::SUCCESS) {
            parse_state = ParseState::DONE;
            ::close(body_fd);
            body_fd = -1;
            request.body_path = body_path;
        }
    
        return result;
    }
    
    HTTPParser::ParseResult::Type HTTPParser::parse_body_chunked( void ) {

        if (chunk_state == ChunkState::CHUNK_SIZE) {

            ParseResult::Type r = scan_line(MAX_HEADER_BLOCK_LEN);
            if (r != ParseResult::SUCCESS) {
                return r;
            }
            
            chunk_remaining = parse_chunk_size(line_buff);
            if (chunk_remaining > MAX_CHUNK_SIZE) {
                return ParseResult::PARSE_ERROR;
            }
            line_buff.clear();
            
            if (chunk_remaining == 0) {
                chunk_state = ChunkState::CHUNK_LAST;
            } else {
                chunk_state = ChunkState::CHUNK_DATA;
            }
        }

        if (chunk_state == ChunkState::CHUNK_LAST) {
            ParseResult::Type res = scan_line(MAX_HEADER_BLOCK_LEN);
            if (res != ParseResult::SUCCESS) {
                return res;
            }
            if (line_buff.size() != 0) {
                return ParseResult::PARSE_ERROR;
            }

            return ParseResult::SUCCESS;
        }

        ::size_t remaining = chunk_remaining - body_bytes_parsed;
        ::size_t to_copy = std::min(remaining, len_ - bytes_consumed);
        ::write(body_fd, data_ + bytes_consumed, to_copy);
        body_bytes_parsed += to_copy;
        bytes_consumed += to_copy;
        
        if (body_bytes_parsed == chunk_remaining) {
            body_bytes_parsed = 0;
            line_buff.clear();
            chunk_state = ChunkState::CHUNK_SIZE;
        }

        return ParseResult::NEED_MORE_BYTES;
    }

    HTTPParser::ParseResult::Type HTTPParser::parse_body_content_length( void ) {
    
        ::size_t remaining = body_len - body_bytes_parsed;
        ::size_t available = len_ - bytes_consumed;
        
        if (available >= MIN_BODY_CHUNK) {
        }

        ::size_t to_copy = std::min(remaining, available);

        ::write(body_fd, data_ + bytes_consumed, to_copy);
        
        body_bytes_parsed += to_copy;
        bytes_consumed += to_copy;

        if (body_bytes_parsed == body_len) {
            return ParseResult::SUCCESS;
        }
        return ParseResult::NEED_MORE_BYTES;
    }
}

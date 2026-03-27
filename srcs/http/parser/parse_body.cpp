
#include "HTTPParser.hpp"
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#define echo std::cout <<
namespace http {

    HTTPParser::ParseResult::Type HTTPParser::parse_body( void ) {

        std::cout << "Body parsing\n";
        std::string body_filename;
        if (BodyType::UNSET == body_type) {

            body_type = detect_body_type();
            echo int(body_type == BodyType::CONTENT_LENGTH) ;
            echo "\n"; 
            if (body_type == BodyType::ERROR) {
                return ParseResult::PARSE_ERROR;
            }
            
            if (body_type == BodyType::NONE) {
                return ParseResult::SUCCESS;
            }
            
            std::stringstream ss;
            ss << conn_fd;
            
           body_filename = body_dir + std::string("/tmp_body_") + ss.str();
            
            body_fd = open(body_filename.c_str(), O_WRONLY | O_CREAT, 0600);
            if (body_fd < 0) {
                return ParseResult::PARSE_ERROR;
            }

        }
                
        if (body_len > MAX_BODY_LEN) {
            return ParseResult::PARSE_ERROR;
        }

        ::size_t remaining = body_len - body_bytes_parsed;
        ::size_t to_copy = std::min(remaining, len_ - bytes_consumed);

        ::write(body_fd, data_ + bytes_consumed, to_copy);
        
        body_bytes_parsed += to_copy;
        bytes_consumed += to_copy;
        if (body_bytes_parsed == body_len) {
            parse_state = ParseState::DONE;
            ::close(body_fd);
            body_fd = -1;
            request.body_path = body_filename;
            return ParseResult::SUCCESS;
        }
        
        return ParseResult::NEED_MORE_BYTES;
    }
}

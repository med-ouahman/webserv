
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
            }
            
            if (body_type == BodyType::NONE) {
                return ParseResult::SUCCESS;
            }
            
            std::stringstream ss;
            ss << conn_fd;
            
            const std::string body_filename = body_dir + std::string("/tmp_body_") + ss.str();
            
            body_fd = open(body_filename.c_str(), O_WRONLY);
            if (0 < body_fd) {
                return ParseResult::PARSE_ERROR;
            }
        }

        if (body_len > MAX_BODY_LEN) {
            return ParseResult::PARSE_ERROR;
        }

        while (true) {

            ::size_t remaining = body_len - body_bytes_parsed;
            ::size_t copy = std::min(remaining, len_ - bytes_consumed);

            ::write(body_fd, data_, copy);
            
            body_bytes_parsed += copy;
            bytes_consumed += copy;
            if (body_bytes_parsed == body_len) {
                break;
            }
        }

        return ParseResult::SUCCESS;
    }

}


#include "HTTPParser.hpp"

namespace http {

    HTTPParser::ParseResult HTTPParser::parse_headers( void ) {
        
        size_t offset = 0;
        while (true) {
            size_t pos = request_buff.find("\r\n", offset);
            if (pos == std::string::npos)
                break;
            if (pos == offset) {
                // std::cout 
                offset += 2;
                headers_done = true;
                break;
            }
            size_t line_len = pos - offset;
            header_bytes_parsed += line_len + 2; // +2 for \r\n
            if (header_bytes_parsed > HEADERS_MAX_LENGTH) {
                parse_state = ERROR;
                std::cout << "Error: headers exceed maximum size\n";
                return PARSE_ERROR;
            }
            std::string header = request_buff.substr(offset, line_len);
            if (!add_request_header(header)) {
                parse_state = ERROR;
                std::cout << "Error adding header\n";
                return PARSE_ERROR;
            }
            offset += line_len + 2;
        }

        if (!headers_done) {
            return;
        }
        
        request_buff.erase(0, offset);
        if (!validate_headers()) {
            parse_state = ERROR;
            std::cout << "Error in validation\n";
            return PARSE_ERROR;
        }
        parse_state = BODY;
        return CONTINUE;
    }
}

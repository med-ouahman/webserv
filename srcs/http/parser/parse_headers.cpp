
#include "HTTPParser.hpp"

namespace http {

    HTTPParser::ParseResult::Type HTTPParser::parse_headers( void ) {

        while (!headers_done) {

            ParseResult::Type r = scan_line(MAX_HEADER_BLOCK_LEN);
            if (r != ParseResult::SUCCESS) {
                return r;
            }

            if (line_buff.size() == 0) {
                headers_done = true;
                break;
            }

            ::size_t name_len = 0;
            ::size_t cursor = 0;
            while (cursor < line_buff.size() && line_buff[cursor] != ':' && name_len <= MAX_HEADER_NAME_LEN) {
                ++cursor;
                ++name_len;
            }
        
            if (cursor == 0 || name_len > MAX_HEADER_NAME_LEN) {
                return ParseResult::PARSE_ERROR;
            }
        
            std::string name = line_buff.substr(0, name_len);
            std::cout << "name len: " << name_len << '\n';
            if (!validate_header_name(name)) {
                return ParseResult::PARSE_ERROR;
            }
            
            normalize_header_name(name);
            ::size_t start = cursor + 1;
            ::size_t end = line_buff.size() - 1;
            
            while (isspace(line_buff[start]) && start < line_buff.size()) ++start;
            
            while (isspace(line_buff[end]) && end >= start) --end;

            request.headers[name] = line_buff.substr(start, end - start);
            line_buff.clear();
        }
    
        validate_headers();
        
        parse_state = ParseState::BODY;
        return ParseResult::ParseResult::SUCCESS;
    }
}


#include "HTTPParser.hpp"

namespace http {

    HTTPParser::ParseResult::Type HTTPParser::parse_headers( void ) {
       
        while (!headers_done) {
            ParseResult::Type r = scan_line(MAX_HEADER_BLOCK_LEN);   
            if (r != ParseResult::SUCCESS) {
                return r;
            }
            ticks_since_progress = 0;
            if (line_buff.size() == 0) {
                headers_done = true;
                break;
            }
            ::size_t name_len = 0;
            ::size_t cursor = 0;
            while (cursor < line_buff.size() && line_buff[cursor] != ':') {
                ++cursor;
                ++name_len;
                if (name_len > MAX_HEADER_BLOCK_LEN) {
                    return ParseResult::PARSE_ERROR;
                }
            }
            if (cursor == line_buff.size() || line_buff[cursor] != ':') {
                return ParseResult::PARSE_ERROR;
            }
        
            std::string name = line_buff.substr(0, name_len);
            if (!validate_header_name(name)) {
                return ParseResult::PARSE_ERROR;
            }
            normalize_header_name(name);
            
            ::size_t start = cursor + 1;
            ::size_t end = line_buff.size() - 1;
            ::size_t __n = 0;
            
            while (::isspace(line_buff[start]) && start < line_buff.size()) ++start;
            while (::isspace(line_buff[end]) && end >= start) --end;

            if (start == end  && !::isspace(line_buff[start])) {
                __n = 1;
            } else {
                __n = end - start + 1;
            }
            request.headers[name] = line_buff.substr(start, __n);
    
            line_buff.clear();
        }
    
        if (!validate_headers()) {
            return ParseResult::PARSE_ERROR;
        }
        
        // std::cout << "Headers:\n";
        // for (std::map<std::string, std::string>::iterator it = request.headers.begin(); it != request.headers.end(); ++it) {
        //     std::cout << (*it).first << ": " << (*it).second << "\n";
        // }
        parse_state = ParseState::BODY;
        return ParseResult::SUCCESS;
    }
}


#include "HTTPParser.hpp"

namespace http {

    ScanResult HTTPParser::parse_headers() {
       
        while (!headers_done) {
            
            if (header_count > MAX_HEADER_COUNT) return ERROR;

            ScanResult r = line_c.scan(MAX_HEADER_BLOCK_LEN);
            
            if (r != SUCCESS) return r;
            
            if (line_c.line().size() == 0) {
                headers_done = true;
                break;
            }
            
            size_t name_len = 0;
            size_t cursor = 0;
            while (cursor < line_c.line().size() && line_c.line()[cursor] != ':') {
                ++cursor;
                ++name_len;
                if (name_len > MAX_HEADER_BLOCK_LEN) return ERROR;
            }

            if (cursor == line_c.line().size() || line_c.line()[cursor] != ':') return ERROR;

            std::string name = line_c.line().substr(0, name_len);
            
            if (!validate_header_name(name)) return ERROR;
        
            normalize_header_name(name);
            
            size_t start = cursor + 1;
            size_t end = line_c.line().size() - 1;
            size_t __n = 0;
            
            while (::isspace(line_c.line()[start]) && start < line_c.line().size()) ++start;
            
            while (::isspace(line_c.line()[end]) && end >= start) --end;

            if (start == end  && !::isspace(line_c.line()[start])) __n = 1;

            else __n = end - start + 1;
    
            request.headers[name] = line_c.line().substr(start, __n);
            ++header_count;
            line_c.reset();
        }
    
        if (!validate_headers()) return ERROR;

        parse_state = ParseState::BODY;
        return SUCCESS;
    }
}

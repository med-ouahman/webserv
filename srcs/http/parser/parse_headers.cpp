
#include "Parser.hpp"

namespace http {

    Base::Expected<std::pair<std::string, std::string>, int> Parser::parse_header( const std::string& line ) {
    
        std::pair<std::string, std::string> header;

        size_t name_len = 0;
        size_t cursor = 0;
        while (cursor < line.size() && line[cursor] != ':') {
            ++cursor;
            ++name_len;
            if (name_len > MAX_HEADER_BLOCK_LEN) return ERROR;
        }

        if (cursor == line.size() || line[cursor] != ':') return ERROR;

        std::string name = line.substr(0, name_len);
        
        if (!validate_http_header_name(name)) return ERROR;
    
        normalize_http_header_name(name);
        
        size_t start = cursor + 1;
        size_t end = line.size() - 1;
        size_t __n = 0;
        
        while (::isspace(line[start]) && start < line.size()) ++start;
        
        while (::isspace(line[end]) && end >= start) --end;

        if (start == end  && !::isspace(line[start])) __n = 1;

        else __n = end - start + 1;

        header.first = name;
        header.second = line.substr(start, __n);
        return header;
    }
}

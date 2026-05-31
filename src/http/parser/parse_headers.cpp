
#include "Parser.hpp"
 #include <iostream>
namespace http {

namespace parser {
Base::Expected<std::pair<std::string, std::string>, int> parse_header(const std::string& line) {
    std::pair<std::string, std::string> header;

    size_t name_len = 0;
    size_t cursor = 0;
    while (cursor < line.size() and line[cursor] not_eq ':') {
        ++cursor;
        ++name_len;
    }
    if (cursor == line.size() or line[cursor] not_eq ':')
    {
        std::cerr << "XXXXXXXXX|\n";
        return 1;
    }
    std::string name = line.substr(0, name_len);
    if (not validate_header_name(name)) return 1;
    normalize_header_name(name);
    size_t start = cursor + 1;
    size_t end = line.size() - 1;
    size_t __n = 0;
    while (::isspace(line[start]) and start < line.size()) ++start;
    while (::isspace(line[end]) and end >= start) --end;
    if (start == end  and !::isspace(line[start])) __n = 1;
    else __n = end - start + 1;
    header.first = name;
    header.second = line.substr(start, __n);
    return header;
}
}
}

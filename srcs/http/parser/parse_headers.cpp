
#include "Parser.hpp"

namespace http {

    Parser::ParseResult Parser::parse_headers( void ) {
        size_t line_index = std::string::npos;
        while ((line_index = request_buff.find("\r\n")) != std::string::npos) {

        }
        return SUCCESS;
    }
}

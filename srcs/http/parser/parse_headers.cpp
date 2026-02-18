
#include "HTTPParser.hpp"

namespace http {

    HTTPParser::ParseResult HTTPParser::parse_headers( void ) {
        
        size_t line_index;
    
        while ((line_index = request_buff.find("\r\n")) != std::string::npos) {
            std::string header = request_buff.substr(0, line_index);
            if (line_index == 0) {
                break;
            }
            if (!add_request_header(header)) {
                parse_state = ERROR;
                std::cout << "Error adding header\n";
                return PARSE_ERROR;
            }
            request_buff = request_buff.substr(line_index + 2);
        }
        if (!validate_headers()) {
            parse_state = ERROR;
            std::cout << "Error in validation\n";
            return PARSE_ERROR;
        }
        std::cout << "Headers done\n";
        for ( std::map<std::string, std::string>::iterator it = request.headers.begin(); it != request.headers.end(); ++it ) {
            std::cout << "key: " << (*it).first << "\nValue: " << (*it).second << "\n";
        }
        parse_state = DONE;
        return SUCCESS;
    }
}


#include "HTTPParser.hpp"

namespace http {

    void HTTPParser::parse_headers( void ) {
        
        size_t line_index;
    
        while ((line_index = request_buff.find("\r\n")) != std::string::npos) {
            if (line_index == 0) {
                request.headers_done = true;
                break;
            }
            std::string header = request_buff.substr(0, line_index);
            if (!add_request_header(header)) {
                parse_state = ERROR;
                std::cout << "Error adding header\n";
                return ;
            }
            /* the next line is fucked up and needs fixing */
            request_buff = request_buff.substr(line_index + 2); // this is O(N) and very bad. remember to add a variable that accumulates the number of bytes consumed and do it once.
        }
        if (!request.headers_done) {
            return ;
        }
        if (!validate_headers()) {
            parse_state = ERROR;
            std::cout << "Error in validation\n";
            return ;
        }
        
        parse_state = BODY;
        std::cout << "Headers done\n";
    }
}

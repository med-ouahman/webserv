
#include "HTTPParser.hpp"

namespace http {
    void HTTPParser::parse_body( void ) {
  
        size_t needed = request.body_len - request.body.length();
        if (needed == 0) {
            parse_state = DONE;
            return ;
        }
        size_t num_copy = std::min(needed, request_buff.size());
        request.body.append(request_buff.substr(0, num_copy));
        request_buff.erase(0, num_copy);
        if (request.body.length() == request.body_len) {
            parse_state = DONE;
            std::cout <<"Request bytes after full parsing: " << request_buff.length() << "\n";
        }
    }
}

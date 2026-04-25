
#include "HTTPDispatcher.hpp"
#include <sstream>
#include <cstring>
namespace http {

    void HTTPDispatcher::serialize_current_header( void ) {

        line_buff = (*current_header).first + COLON + (*current_header).second + CRLF;
    }

    ::size_t HTTPDispatcher::serialize_headers( char* buff, ::size_t size ) {
        ::size_t bytes = 0;
        
        if (serialize_state == RESPONSE_LINE) {
            std::stringstream ss;
            ss << response.status_code;
        
            line_buff = "HTTP\\1.1 " + ss.str() + " " + response.reason + CRLF;
            ::memcpy(buff, line_buff.c_str(), line_buff.size());
            bytes += line_buff.size();
            serialize_state = HEADERS;
            line_buff.clear();
            current_header = response.headers.begin();
        }
        
        if (HEADERS == serialize_state) {
            
            if (current_header == response.headers.end()) {
                ::memcpy(buff, CRLF, CRLF_SIZE);
                bytes += CRLF_SIZE;
                serialize_state = BODY;
            }

            for ( ; current_header != response.headers.end(); ) {
                
                if (line_buff.size() == 0) {
                    line_buff = (*current_header).first + COLON + (*current_header).second + CRLF;
                } 
                
                ::size_t available = size > bytes ? size - bytes: 0;
                ::size_t remaining = std::min(available, line_buff.size());
                ::memcpy(buff, line_buff.c_str(), remaining);
                bytes += remaining;
                line_buff.erase(remaining);

                if (line_buff.size() != 0)
                    break;
                else
                    ++current_header;

            }
        }

        return bytes;
    }
}

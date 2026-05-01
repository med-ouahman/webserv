
#include "HTTPResponse.hpp"
#include <sstream>
#include <cstring>
namespace http {

    void HTTPResponse::serialize_current_header( void ) {

        line_buff = (*current_header).first + COLON + (*current_header).second + CRLF;
    }

    ::size_t HTTPResponse::serialize_headers( char* buff, ::size_t size ) {
        ::size_t bytes = 0;
        
        if (serialize_state == RESPONSE_LINE) {
            std::stringstream ss;
            ss << status_code;
        
            line_buff = "HTTP\\1.1 " + ss.str() + " " + reason + CRLF;
            ::memcpy(buff, line_buff.c_str(), line_buff.size());
            bytes += line_buff.size();
            serialize_state = HEADERS;
            line_buff.clear();
            current_header = headers.begin();
        }
        
        if (HEADERS == serialize_state) {
            
            if (current_header == headers.end()) {
                ::memcpy(buff, CRLF, CRLF_SIZE);
                bytes += CRLF_SIZE;
                serialize_state = BODY;
            }

            for ( ; current_header != headers.end(); ) {
                
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

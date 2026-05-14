
#include "HTTPResponse.hpp"
#include <sstream>
#include <cstring>
#include "BufferWriter.hpp"

namespace http {

    const char* HTTPResponse::COLON = ":";
    const char* HTTPResponse::CRLF = "\r\n";
    
    size_t HTTPResponse::serialize_headers( BufferWriter* writer ) {
        serialize_state = BODY;
        if (serialize_state == RESPONSE_LINE) {
            std::cout<< "Serializing Response Line\n";
            std::stringstream ss;
            ss << status_code;
        
            line_buff = "HTTP/1.1 " + ss.str() + " " + reason + CRLF;
            ::memcpy(writer->write_ptr(), line_buff.c_str(), line_buff.size());
            writer->advance(line_buff.size());
            serialize_state = HEADERS;
            line_buff.clear();
            current_header = headers.begin();
        }
        
        if (HEADERS == serialize_state) {
            std::cout << "Serializing Headers\n";
            if (current_header == headers.end()) {
                ::memcpy(writer->write_ptr(), CRLF, CRLF_SIZE);
                writer->advance(CRLF_SIZE);
                serialize_state = BODY;
            }

            while (current_header != headers.end()) {
                
                if (line_buff.size() == 0) {
                    line_buff = (*current_header).first + COLON + (*current_header).second + CRLF;
                }
                
                size_t remaining = std::min(writer->remaining(), line_buff.size());
                ::memcpy(writer->write_ptr(), line_buff.c_str(), remaining);
                writer->advance(remaining);

                line_buff.erase(remaining);

                if (line_buff.size() != 0)
                    break;
                else
                    ++current_header;

            }
        }

        return writer->size();
    }
}

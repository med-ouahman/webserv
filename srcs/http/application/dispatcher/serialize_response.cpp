
#include "HTTPResponse.hpp"
#include <sstream>
#include <cstring>
#include "BufferWriter.hpp"

namespace http {

    const char* HTTPResponse::COLON = ": ";
    const char* HTTPResponse::CRLF = "\r\n";
    
    size_t HTTPResponse::serialize_headers( BufferWriter* writer ) {
        if (serialize_state == RESPONSE_LINE) {
            std::cout<< "Serializing Response Line\n";
            std::stringstream ss;
            ss << status_code;
        
            line_buff = "HTTP/1.1 " + ss.str() + " " + reason + CRLF;
            std::cout << line_buff;
            writer->write(line_buff.c_str(), line_buff.size());
            serialize_state = HEADERS;
            line_buff.clear();
            current_header = headers.begin();
        }
        
        if (HEADERS == serialize_state) {
            std::cout << "Serializing Headers\n";
            if (current_header == headers.end()) {
                
                if (writer->remaining() >= CRLF_SIZE) {
                    std::cout << "Response Headers done\n";
                    writer->write(CRLF, CRLF_SIZE);
                    serialize_state = BODY;
                    return 0;
                }
            }

            while (current_header != headers.end()) {
                
                if (line_buff.size() == 0) {
                    line_buff = (*current_header).first + COLON + (*current_header).second + CRLF;
                }
                
                std::cout << "header size: "<< line_buff.size() <<"\n";
                size_t w = writer->write(line_buff.c_str(), line_buff.size());

                std::cout << "Bytes written: " << w << "\n";
                line_buff.erase(0, w);
                std::cout << line_buff.size() << "\n";
                if (line_buff.size() != 0) {
                    std::cout << "leftovers";
                    return 0;
                    break;
                }
                else {
                    std::cout << "next header\n";
                    ++current_header;
                }

            }
        }

        return writer->size();
    }
}

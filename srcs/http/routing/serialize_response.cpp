
#include "Response.hpp"
#include <sstream>
#include <cstring>
#include "BufferWriter.hpp"

namespace http {

    const char* Response::COLON = ": ";
    const char* Response::CRLF = "\r\n";
    
    size_t Response::serialize( BufferWriter* writer ) {
        if (serialize_state == RESPONSE_LINE) {
            std::cout<< "Serializing Response Line\n";
            std::stringstream ss;
            ss << status_code;
        
            line_buff = "HTTP/1.1 " + ss.str() + " " + "OK" + CRLF;
            std::cout << line_buff;
            writer->write(line_buff.c_str(), line_buff.size());
            serialize_state = HEADERS;
            line_buff.clear();
            current_header = headers.begin();
        }
        
        if (HEADERS == serialize_state) {
            std::cout << "Serializing Headers\n";

            while (current_header != headers.end()) {
                
                if (line_buff.size() == 0) line_buff = (*current_header).name + COLON + (*current_header).value + CRLF;
                
                size_t w = writer->write(line_buff.c_str(), line_buff.size());

                line_buff.erase(0, w);

                if (not line_buff.empty()) break;

                else ++current_header;
            }

            if (current_header == headers.end()) {
                
                if (writer->remaining() >= CRLF_SIZE) {
                    std::cout << "Response Headers done\n";
                    writer->write(CRLF, CRLF_SIZE);
                    serialize_state = BODY;
                }
            }
        }

        return writer->size();
    }
}

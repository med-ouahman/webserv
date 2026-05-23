
#include "Response.hpp"
#include <sstream>
#include <cstring>
#include "BufferWriter.hpp"
#include <iostream>

namespace http {

    const char* Response::COLON = ": ";
    const char* Response::CRLF = "\r\n";
    
    void Response::serialize_response_line( BufferWriter& writer ) {
        std::stringstream ss;
        
        ss << status_code;
    
        line_buff = "HTTP/1.1 " + ss.str() + " " + "OK" + CRLF;

        std::cout << line_buff;
        
        writer.write(line_buff.c_str(), line_buff.size());
        
        state = HEADERS;
        
        line_buff.clear();
        
        current_header = headers.begin();
    }

    void Response::serialize_headers( BufferWriter& writer ) {
        std::cout << "Serializing Headers\n";

        while (current_header != headers.end()) {
            
            if (line_buff.size() == 0) line_buff = (*current_header).name + COLON + (*current_header).value + CRLF;
            
            size_t w = writer.write(line_buff.c_str(), line_buff.size());

            line_buff.erase(0, w);

            if (not line_buff.empty()) break;

            else ++current_header;
        }

        if (current_header == headers.end()) {
            
            if (writer.remaining() >= CRLF_SIZE) {
                std::cout << "Response Headers done\n";
                writer.write(CRLF, CRLF_SIZE);
                state = BODY;
            }
        }
    }

    size_t Response::serialize( BufferWriter& writer ) {
        
        if (state == RESPONSE_LINE) serialize_response_line(writer);
        
        if (HEADERS == state) serialize_headers(writer);

        return writer.size();
    }
}

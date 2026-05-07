#include "CGIBodyProvider.hpp"
#include "CGIHandler.hpp"
#include <sstream>

namespace http {

    CGIBodyProvider::CGIBodyProvider( CGIHandler& h, BodySendMethod::Type b, size_t size )
        : body_size(size),
        body_bytes_read(0),
        cgi_handler(h),
        data_view(h.get_stdout_data_view()),
        send_method(b)
    {}

    CGIBodyProvider::~CGIBodyProvider() {

    }

    bool CGIBodyProvider::finished() const {
        return true;
    }

    ssize_t CGIBodyProvider::read( char* buff, size_t size ) {
 
        if (leftover == 0) {
            cgi_handler.pull();
            if (data_view.size() == 0) return 0;
        }
        
        
        if (cgi_handler.get_cgi_state() != CGIState::WRITING_BODY) {
            return -1;
        }

        body_bytes_read += data_view.size();

        if (body_bytes_read > body_size)
            return -1;

        if (send_method == BodySendMethod::CONTENT_LENGTH) {
            
            ssize_t to_copy = std::min(data_view.size() - data_view.cursor(), size);
            ::memcpy(buff, data_view.data(), to_copy);
            return to_copy;
        }

        size_t copied = 0;

        std::string formatted = format_chunk(data_view.size());

        ::memcpy(buff, formatted.c_str(), formatted.size());
        buff += formatted.size();
        copied += formatted.size();
        size_t rem = size - formatted.size();
        size_t to_copy = std::min(rem, data_view.size());
        ::memcpy(buff, data_view.data(), to_copy);
        buff += to_copy;
        ::memcpy(buff, "\r\n", 2);
        
        return copied;
    }

    std::string CGIBodyProvider::format_chunk( size_t chunk_size ) {
        std::stringstream ss;
        ss << std::hex << chunk_size;

        std::string formatted = ss.str();
        formatted.append("\r\n");

        return formatted;
    }

   
}

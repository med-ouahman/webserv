#include "BodyParser.hpp"
#include <unistd.h> 

#include <iostream>

namespace http {

    ScanResult BodyParser::parse_body_content_length() {

        write_body();

        if (body_bytes_parsed == body_len) body_state = BodyState::FINISH;
        
        return SUCCESS;
    }


    void BodyParser::write_body() {

        size_t remaining = body_len - body_bytes_parsed;
        size_t available = view->len_ - view->bytes_consumed;
        
        size_t to_copy = std::min(remaining, available);

        ssize_t copied;

        if (body_storage == BodyStorage::BUFFER) {
            body_buff.append(view->data_ptr_ + view->bytes_consumed, to_copy);
            copied = to_copy;
        } else {
            copied = ::write(body_fd, view->data_ptr_ + view->bytes_consumed, to_copy); 
            if (copied < 0) {
                body_state = BodyState::ERROR;
                return ;
            }
        }

        body_bytes_parsed += copied;
        view->bytes_consumed += copied;
    }
}

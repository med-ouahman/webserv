#include "BodyHandler.hpp"
#include <unistd.h> 

#include <iostream>

namespace http {

    ScanResult BodyHandler::parse_body_content_length() {

        write_body();

        if (body_bytes_parsed == body_len) body_state = BodyState::FINISH;
        
        return SUCCESS;
    }


    void BodyHandler::write_body() {

        size_t remaining = body_len - body_bytes_parsed;
        size_t available = data_view->len_ - data_view->bytes_consumed;
        
        size_t to_copy = std::min(remaining, available);

        ssize_t copied;

        if (body_storage == BodyStorage::BUFFER) {
            body_buff.append(data_view->data_ptr_ + data_view->bytes_consumed, to_copy);
            copied = to_copy;
        } else {
            copied = ::write(body_fd, data_view->data_ptr_ + data_view->bytes_consumed, to_copy); 
            if (copied < 0) {
                body_state = BodyState::ERROR;
                return ;
            }
        }

        body_bytes_parsed += copied;
        data_view->bytes_consumed += copied;
    }
}

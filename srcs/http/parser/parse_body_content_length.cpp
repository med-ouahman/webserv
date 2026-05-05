#include "BodyHandler.hpp"
#include <unistd.h> 

#include <iostream>

namespace http {

    ScanResult BodyHandler::parse_body_content_length() {

        write_body();
        
        if (body_bytes_parsed == body_len) body_state = BodyState::FINISH;

        switch (body_state) {
            case BodyState::ERROR:
                return ERROR;
            case BodyState::FINISH:
                return SUCCESS;
            case BodyState::READING_BODY:
                return NEED_MORE;
            default:
                return NEED_MORE;
        }

        return SUCCESS;
    }


    void BodyHandler::write_body() {

        size_t remaining = body_len - body_bytes_parsed;
        size_t available = data_view.size() - data_view.cursor();
        
        size_t to_copy = std::min(remaining, available);

        ssize_t copied;

        if (body_storage == BodyStorage::BUFFER) {
            body_buff.append(data_view.data() + data_view.cursor(), to_copy);
            copied = to_copy;
        } else {
            copied = ::write(body_fd, data_view.data() + data_view.cursor(), to_copy); 
            if (copied < 0) {
                body_state = BodyState::ERROR;
                return ;
            }
        }

        body_bytes_parsed += copied;
        data_view.advance(copied);
    }
}

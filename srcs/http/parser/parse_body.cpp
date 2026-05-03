
#include "BodyHandler.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <iostream>

namespace http {
    
    bool BodyHandler::is_valid_hexa( const char c ) {
		return hexas.find(c) != std::string::npos;
	}
    
   ScanResult BodyHandler::parse_body() {

        if (!body_set) prepare_body();

        ScanResult result;
        switch (body_type) {
            case BodyType::CONTENT_LENGTH:
                result = parse_body_content_length();
                break;
            case BodyType::TRANSFER_ENCODING_CHUNKED:
                result = parse_body_chunked();
                break;
            case BodyType::ERROR:
                result = ERROR;
                break;
            case BodyType::NONE:
                result = SUCCESS;
            default:
                break;
        }

        if (body_state == BodyState::ERROR) return ERROR;
        
        if (result == SUCCESS) reset();

        return result;
    }

    void BodyHandler::reset() {

        if (body_fd >= 0) {
            ::close(body_fd);
            body_fd = -1;
        }

    }
}

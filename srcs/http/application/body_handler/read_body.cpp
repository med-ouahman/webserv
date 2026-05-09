
#include "BodyHandler.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <iostream>

namespace http {
    
    bool BodyHandler::is_valid_hexa( const char c ) {
		return hexas.find(c) != std::string::npos;
	}
    
   ScanResult BodyHandler::read_body( const std::string& filename ) {

        if (body_storage == BodyStorage::NONE) prepare_body(filename);

        ScanResult result;
        switch (body_type) {
            case BodyType::CONTENT_LENGTH:
                result = read_body_content_length();
                break;
            case BodyType::TRANSFER_ENCODING_CHUNKED:
                result = read_body_chunked();
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

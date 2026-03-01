
#include "Connection.hpp"
#include <cerrno>

namespace core {

    bool Connection::update_buff( ssize_t sent_bytes ) {

        if (sent_bytes < 0) {
            if (errno != EAGAIN) {
                state = CLOSING;
            }
            return false;
        }

        sent_offset += sent_bytes;
        
        if (sent_offset >= bytes_in_buff) {
            sent_offset = 0;
            if (advance()) {
                return true;
            }
            state = close_after_write ? CLOSING : READING;
        }

        return true;
    }

    const char* Connection::get_write_buff( void ) const {
        return buff + sent_offset;
    }

    size_t Connection::get_sent_offset( void ) const {
        return sent_offset;
    }

}

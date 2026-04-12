
#include "Connection.hpp"
#include <cerrno>
#include <cerrno>
#include <string.h>
namespace core {

    bool Connection::has_data( ::ssize_t sent_bytes ) {

        if (sent_bytes < 0) {
            if (errno != EAGAIN) {
                state = CLOSING;
            }
            return false;
        }

        sent_offset += sent_bytes;
        bytes_in_buff -= sent_bytes;
        if (0 == bytes_in_buff) {
            sent_offset = 0;
            if (advance()) {
                return true;
            }
            if (close_after_write) {
                state = CLOSING;
            } else {
                state = READING;
            }
            return false;
        }

        return true;
    }

    const char* Connection::get_write_buff( void ) const {
        return output_buff + sent_offset;
    }

    size_t Connection::bytes_remaining( void ) const {
        return bytes_in_buff;
    }

}

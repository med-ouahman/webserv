
#include "Connection.hpp"
#include <cerrno>
#include <cerrno>
#include <string.h>

namespace core {

    bool Connection::on_write( void ) {
        
        if (sent_bytes < 0) {
            return false;
        }
    
        sent_offset += sent_bytes;
        bytes_in -= sent_bytes;
        if (0 == bytes_in) {
            sent_offset = 0;
            if (advance()) {
                return true;
            }
            if (close_after_write) {
                state = ConnectionState::CLOSING;
            }
            return false;
        }

        return true;
    }

}

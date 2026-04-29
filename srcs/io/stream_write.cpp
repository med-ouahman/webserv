#include "Stream.hpp"

namespace io {
      
    void Stream::write( void ) {
        
        while (true) {

            if (sent_offset < bytes_to_write) {
                size_t remaining = bytes_to_write - sent_offset;
                bytes_r = ::write(fd, writebuff + sent_offset, remaining);
            }

            if (!process()) {
                break;
            }
        }
    }
}
#include "Stream.hpp"

namespace io {
       void Stream::read( void ) {

        while (true) {

            if (readbuf_drained()) {
                bytes_r = ::read(fd, readbuf, READ_BUFFER_SIZE);
            }

            if (!process()) {
                break;
            }
        }
    }
}

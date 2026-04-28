#include "Stream.hpp"
#include <unistd.h>

namespace io {
    
    Stream::Stream()
        :bytes_in(0),
        bytes_received(0) {}

    void Stream::read( void ) {

        while (true) {
            if (bytes_received == 0) {
                bytes_received = ::read(fd, readbuf, READ_BUFFER_SIZE);
                if (bytes_received < 0) {
                    break;
                }
            }
            process();
        }
    }
    
    void Stream::write( void ) {
        while (true) {
            if (bytes_in == sent_bytes) {
                bytes_in = ::write(fd, writebuff, SEND_CHUNK_SIZE);
                if (bytes_in < 0) {
                    break;
                }
            }
            process();
        }
    }

}
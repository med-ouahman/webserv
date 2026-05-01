#include "Stream.hpp"
#include "Result.hpp"

namespace io {

    void Stream::read( void ) {
        bytes_r = ::read(fd, readbuf, READ_BUFFER_SIZE);
    }
    
}

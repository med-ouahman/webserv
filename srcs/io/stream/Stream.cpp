#include "Stream.hpp"

namespace io {
    
    Stream::Stream( int fd )
        : IIOHandler(fd),
        processing(false),
        bytes_r(0),
        bytes_received(0),
        bytes_consumed(0),
        bytes_to_write(0),
        bytes_sent(0),
        sent_offset(0) {}
}
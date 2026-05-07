#include "Stream.hpp"

namespace io {
    
    Stream::Stream( int fd )
        : IIOHandler(fd),
        processing(false),
        bytes_r(0),
        readbuf(),
        writebuff(),
        bytes_to_write(0),
        bytes_sent(0),
        sent_offset(0),
        data_view(readbuf),
        writer(writebuff) {}
}
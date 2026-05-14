#include "Stream.hpp"

namespace io {
    
    Stream::Stream( int fd )
        : IIOHandler(fd),
        readbuf(),
        writebuff(),
        processing(false),
        bytes_r(0),
        data_view(readbuf),
        writer(writebuff, WRITE_BUFFER_SIZE) {}
}
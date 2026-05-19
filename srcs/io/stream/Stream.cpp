#include "Stream.hpp"

namespace io {
    
    Stream::Stream( int fd )
        : AEventHandler(fd),
        readbuf(),
        writebuff(),
        bytes_r(0),
        data_view(readbuf),
        writer(writebuff, WRITE_BUFFER_SIZE) {}
}
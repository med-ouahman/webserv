#include "Stream.hpp"

namespace io {
    
    Stream::Stream( int fd )
        : IIOHandler(fd),
        processing(false),
        bytes_r(0),
        readbuf(),
        writebuff(),
        data_view(readbuf),
        writer(writebuff, SEND_CHUNK_SIZE) {}
}
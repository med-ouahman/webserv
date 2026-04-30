#include "Stream.hpp"
#include <string.h>
namespace io {
      
    void Stream::write( void ) {
        size_t remaining = bytes_to_write - sent_offset;
        bytes_r = ::write(fd, writebuff + sent_offset, remaining);
        if (bytes_r < 0) {
            LOG_ERROR(MAKE_ERRNO_ERROR("Stream::write()"));
        }
    }
}
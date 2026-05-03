#include "Stream.hpp"
#include <string.h>
namespace io {
    
    void Stream::write() {
        size_t remaining = bytes_to_write - sent_offset;
        bytes_r = ::write(fd, writebuff + sent_offset, remaining);
    }
}
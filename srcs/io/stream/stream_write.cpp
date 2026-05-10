#include "Stream.hpp"
#include <string.h>
namespace io {
    
    void Stream::write() {
        bytes_r = ::write(fd, writer.data(), writer.remaining());
    }
}
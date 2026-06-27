

#include "Connection.hpp"
#include <sys/socket.h>

namespace net {

void Connection::read() {

    reader_.compact();
    ssize_t n = ::recv(fd(), reader_.write_ptr(), reader_.bytes_free(), 0);
    
    if (n <= 0) {
        state_ = Closing;
        return;
    }
    
    reader_.advance_write(n);
}

void Connection::write() {

    ssize_t n = ::send(fd(), writer_.read_ptr(), writer_.bytes_pending(), 0);

    if (n < 0) {
        state_ = Closing;
        return;
    }

    writer_.advance_read(n);
    writer_.compact();
}

}

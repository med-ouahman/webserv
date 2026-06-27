

#include "Connection.hpp"
#include <sys/socket.h>

namespace net {

void Connection::read() {

    ssize_t n = ::recv(fd(), reader_.write_ptr() + reader_.write_offset(), reader_.bytes_pending(), 0);
    
    if (n <= 0) {
        state_ = Closing;
        return;
    }
    
    rcvbuf.size_ += n;
}

void Connection::write() {

    
    ssize_t n = ::send(fd(), writer_.read_ptr() + writer_.read_offset(), writer_. , 0);

    if (n < 0) {
        state_ = Closing;
        return;
    }

    sndbuf.size_ -= n;
}

}



#include "Connection.hpp"
#include <sys/socket.h>

namespace net {

void Connection::read() {

    ssize_t n = ::recv(fd(), rcvbuf.data_ + rcvbuf.offset_, rcvbuf.capacity_ - rcvbuf.size_, 0);
    
    if (n <= 0) {
        state_ = Closing;
        return;
    }
    
    rcvbuf.size_ += n;
}

void Connection::write() {

    
    ssize_t n = ::send(fd(), sndbuf.data_ + sndbuf.offset_, sndbuf.size_ ,0);

    if (n < 0) {
        state_ = Closing;
        return;
    }

    sndbuf.size_ -= n;
}

}

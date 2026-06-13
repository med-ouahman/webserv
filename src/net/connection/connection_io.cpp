/*
    Copyright Webamination 2026 Inc.
*/

#include "Connection.hpp"
#include <sys/socket.h>

namespace net {

void Connection::read() {

    ssize_t n = ::recv(fd(), reader_.write_ptr(), reader_.capacity(), 0);
    
    if (n <= 0) {
        state_ = Closing;
        return;
    }
    
    reader_.update(n);
}

void Connection::write() {

    ssize_t n = ::send(fd(), writer_.read_ptr(), writer_.bytes_pending(), 0);

    if (n < 0) {
        state_ = Closing;
        return;
    }

    writer_.advance_read(n);
}

}

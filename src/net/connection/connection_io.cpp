

#include "Connection.hpp"
#include <sys/socket.h>

namespace net {

void Connection::read() {

    reader_.compact();
    ssize_t n = ::recv(fd(), rdbuf.data_, rdbuf, 0);
    
    if (n <= 0) {
        state_ = Closing;
        return;
    }
    
    reader_.update(n);
}

void Connection::write() {

    writer_.compact();

    ssize_t n = ::send(fd(), writer_.read_ptr(), writer_.bytes_pending(), 0);

    if (n < 0) {
        state_ = Closing;
        return;
    }

    writer_.advance_read(n);
    
    if (writer_.written() == writer_.size()) writer_.reset();
}

}

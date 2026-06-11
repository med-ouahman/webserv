
#include "Connection.hpp"
#include "Context.hpp"
#include "Dispatcher.hpp"
#include <iostream>
#include <sys/socket.h>

namespace net {

Connection::Connection(int _fd, io::Event events)
    : AEventHandler(_fd, events),
    state_(Reading),
    close_after_write(false),
    last_activity_(),
    lifetime_(),
    ctx() {}

Connection::~Connection() {
    /* cp */
}

ConnectionState Connection::state() const {
    return state_;
}

void Connection::on_event(io::Event events) {
   
    switch (events) {
        
        case io::Readable:
            read();
            ctx.consume(reader_.data(), reader_.size());
            reader_.reset();
            break;
        case io::Writable:
            write();
            break;
        case io::Hup: case io::RHup:
            state_ = Closing;
            break;
        case io::Error:
            /* need log */
            state_ = Closing;
            break;
        default:
            break;
    }

}

bool Connection::closing() const {
    return state_ == Closing;
}


void Connection::read() {

    ssize_t n = ::recv(fd(), reader_.data(), reader_.capacity(), 0);
    
    if (n <= 0) {
        state_ = Closing;
        return;
    }
    std::cout << "N: " << n << '\n';
    std::cout << reader_.data();
    reader_.advance(n);

}

void Connection::write() {
    ssize_t n = ::send(fd(), writer_.write_ptr(), writer_.bytes_pending(), 0);
    
    if (n < 0) {
        state_ = Closing;
        return;
    }

    writer_.advance_write(n);

}



}

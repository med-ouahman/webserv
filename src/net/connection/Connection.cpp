
#include "Connection.hpp"
#include "Context.hpp"
#include "Dispatcher.hpp"
#include <iostream>
#include <sys/socket.h>
#include "Server.hpp"
#include <cstdlib>

namespace net {

Connection::Connection(int _fd, io::Event events, ServerContext& server_ctx)
    : AEventHandler(_fd, events),
    state_(Reading),
    close_after_write(false),
    last_activity_(),
    lifetime_(),
    ctx(server_ctx) {
}

Connection::~Connection() {
    
}

ConnectionState Connection::state() const {
    return state_;
}

void Connection::on_event(io::Event events) {
   
    switch (events) {
        case io::Readable:
            on_readable();
            break;
        case io::Writable:
            on_writable();
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

    update(ctx.next_action());
}

bool Connection::closing() const {
    return state_ == Closing;
}

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

void Connection::on_readable() {
    read();
    
    if (state_ == Closing) return;

    ctx.consume(reader_.data(), reader_.size());

}

void Connection::on_writable() {
    ctx.produce(writer_);
    write();
}

void Connection::update(http::ContextAction action) {

    switch (action) {
        case http::AC_READ:
            state_ = Reading;
            break;
        case http::AC_WRITE:
            state_ = Writing;
            break;
        case http::AC_CLOSE:
            state_ = Closing;
            break;
        default:
            break;
    }

    io::Event new_events = events();
    switch (state_) {
        case Reading:
            new_events = io::Readable;
            break;
        case Writing:
            new_events = io::Writable;
            break;
        case Closing:
            new_events = io::Close;
            break;
    }

    if (close_after_write) new_events = io::Close;

    if (new_events != events()) update_events(new_events);
}

}

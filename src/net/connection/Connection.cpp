
#include "Connection.hpp"
#include "Context.hpp"
#include "Dispatcher.hpp"
#include <iostream>
#include "Server.hpp"
#include <sys/socket.h>

namespace net {

Connection::Connection(int _fd, io::Event events, ServerContext& server_ctx, const ConnectionInfo& info)
    : AEventHandler(_fd, events),
    state_(Reading),
    close_after_write(false),
    last_activity_(),
    lifetime_(),
    ctx(server_ctx),
    reader_(in),
    writer_(out),
    info_(info) {

}

Connection::~Connection() {}

bool Connection::closing() const { return state_ == Closing; }

void Connection::update(http::ContextAction action) {

    if (state_ == Closing && http::AC_CLOSE != action) action = http::AC_CLOSE;

    switch (action) {
        case http::AC_READ: state_ = Reading; break;
        case http::AC_WRITE: state_ = Writing; break;
        case http::AC_CLOSE: state_ = Closing; break;
        default: break;
    }

    io::Event new_events = events();

    switch (state_) {
        case Reading: new_events = io::Readable; break;
        case Writing: new_events = io::Writable; break;
        case Closing: new_events = io::Close; break;
    }

    if (new_events != events()) update_events(new_events);
}

void Connection::sync() {
    
    ctx.reconcile();

    http::ContextAction action = ctx.next_action();

    update(action);
}


void Connection::on_event(io::Event events) {
   
    switch (events) {
        case io::Readable:
            std::cout << "Connection Readable\n";
            on_readable();
            break;
        case io::Writable:
            std::cout << "Connection Writable\n";
            on_writable();
            break;
        case io::Hup: case io::RHup:
            std::cout << "Connection Hangup\n";
            state_ = Closing;
            break;
        case io::Error:
            std::cout << "Channel Error\n";
            /* need log */
            state_ = Closing;
            break;
        default:
            break;
    }
    
}

void Connection::on_readable() {
    read();
    
    if (state_ == Closing) return;

    ctx.consume(reader_.read_ptr(), reader_.size());
}

void Connection::on_writable() {

    if (state_ == Closing) return;

    size_t n = ctx.produce(writer_.write_ptr(), writer_.bytes_free());

    if (ctx.next_action() == http::AC_CLOSE) {
        state_ = Closing;
        std::cout  << "Error\n";
        return;
    }

    writer_.advance_write(n);
    
    write();
}



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

    std::cout << "write: " << n << "\n";
    writer_.advance_read(n);
    writer_.compact();
    
}

}

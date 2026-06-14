
#include "Connection.hpp"
#include "Context.hpp"
#include "Dispatcher.hpp"
#include <iostream>
#include "Server.hpp"

namespace net {

Connection::Connection(int _fd, io::Event events, ServerContext& server_ctx)
    : AEventHandler(_fd, events),
    state_(Reading),
    close_after_write(false),
    last_activity_(),
    lifetime_(),
    ctx(server_ctx),
    current_action(ctx.next_action()) {}

Connection::~Connection() {}

ConnectionState Connection::state() const { return state_; }

bool Connection::closing() const { return state_ == Closing; }

void Connection::update(http::ContextAction action) {
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

    if (new_events != events()) {
        update_events(new_events);
        std::cout << "Updated Connection Events\n";
    }
}

void Connection::sync() {
    
    http::ContextAction next = ctx.next_action();
    if (next == current_action) return;

    current_action = next;
    update(current_action);
}

}

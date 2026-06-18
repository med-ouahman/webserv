
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

Connection::~Connection() {
    
}

ConnectionState Connection::state() const { return state_; }

bool Connection::closing() const { return state_ == Closing; }

void Connection::update(http::ContextAction action) {

    ctx.print_context_action(action);
    Connection* c = this;
    c->print_connection_state(state_);

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

    if (new_events != events()) {
        update_events(new_events);
        std::cout << "Updated Connection Events\n";
    }
}

void Connection::sync() {
    
    ctx.refresh_state();

    http::ContextAction next = ctx.next_action();

    current_action = next;

    update(current_action);

}



void Connection::print_connection_state(ConnectionState state)
{
    switch (state) {
    case Reading:
        std::cout << "ConnectionState::Reading\n";
        break;
    case Writing:
        std::cout << "ConnectionState::Writing\n";
        break;
    case Closing:
        std::cout << "ConnectionState::Closing\n";
        break;
    default:
        std::cout << "Unknown ConnectionState\n";
        break;
    }
}

}

namespace http {
void Context::print_context_action(ContextAction action) {
switch (action) {
    case AC_READ:
    std::cout << "ContextAction::AC_READ\n";
    break;
    case AC_WORK:
    std::cout << "ContextAction::AC_WORK\n";
    break;
    case AC_WRITE:
    std::cout << "ContextAction::AC_WRITE\n";
    break;
    case AC_CLOSE:
    std::cout << "ContextAction::AC_CLOSE\n";
    break;
    default:
    std::cout << "Unknown ContextAction\n";
    break;
}
}

}

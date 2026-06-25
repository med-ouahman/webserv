
#include "Connection.hpp"
#include "Context.hpp"
#include "Dispatcher.hpp"
#include <iostream>
#include "Server.hpp"

namespace net {

Connection::Connection(int _fd, io::Event events, ServerContext& server_ctx, const ConnectionInfo& info)
    : AEventHandler(_fd, events),
    state_(Reading),
    close_after_write(false),
    last_activity_(),
    lifetime_(),
    ctx(server_ctx),
    current_action(ctx.next_action()),
    rcvbuf(in),
    sndbuf(out)
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

    http::ContextAction next = ctx.next_action();

    current_action = next;

    update(current_action);

}

}

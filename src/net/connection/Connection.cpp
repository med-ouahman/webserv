
#include "Connection.hpp"
#include "Context.hpp"
#include "Dispatcher.hpp"

namespace net {

Connection::Connection(int _fd, io::Event events)
    : AEventHandler(_fd, events),
    state_(Reading),
    close_after_write(false),
    last_activity_(),
    lifetime_(),
    ctx(),
    reader_(ReadbufSize),
    writer_(WritebufSize) {}

Connection::~Connection() {
    /* cp */
}


ConnectionState Connection::state() const {
    return state_;
}

void Connection::on_event(io::Event events) {
    switch (events) {
        
        case io::Writable:
            break;
        case io::Readable:
            break;
        case io::Hup: case io::RHup:
            break;

        case io::Error:
            break;
        default:
            break;
    }
}

bool Connection::closing() const {
    return state_ == Closing;
}

}

#include "Connection.hpp"
#include "IRequestHandler.hpp"

namespace net {

void Connection::on_stream_error() {
    state_ = CLOSING;
}

void Connection::on_stream_closed() {
    state_ = CLOSING;
}

io::Stream& Connection::stream() {
    return stream_;
}

void Connection::update_stream() {
    switch (state_) {
        case READING:
            stream_.update_events(io::READABLE);
            break;
        case WRITING:
            stream_.update_events(io::WRITABLE);
            break;
        case CLOSING:
            stream_.update_events(io::CLOSE);
            break;
        default:
            break;
    }
}

}

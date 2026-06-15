#include "Channel.hpp"
#include "CgiHandler.hpp"

namespace http {

Channel::Channel(Stream s, int fd, io::Event events, CgiHandler& h)
  : AEventHandler(fd, events),
    stream_(s),
    state_(Open),
    handler_(h) {}

Channel::~Channel() {}

void Channel::on_event(io::Event event) {

    if (handler_.state() == CgiHandler::Error || handler_.finished()) {
        state_ = Closed;
    }

    if (state_ == Closed || state_ == Error) return;

    switch (event) {
        case io::Readable: case io::Hup:
            handler_.on_readable(reader_, *this);
            break;
        case io::Writable:
            handler_.on_writable(writer_, *this);
            write();
            break;
        case io::RHup:
            state_ = Closed;
            handler_.on_ch_closed(*this);
            break;
        case io::Error:
            state_ = Closed;
            handler_.on_ch_error(*this);
        default:
            break;
    }
}

Channel::Stream Channel::stream() const {
    return stream_;
}

void Channel::read() {
    
    ssize_t n = ::read(fd(), reader_.write_ptr(), reader_.capacity());

    if (n < 0) {
        state_ = Error;
    }

    if (n == 0) state_ = Closed;
    
    reader_.update(n);
}

void Channel::write() {
    ssize_t n = ::write(fd(), writer_.read_ptr(), writer_.bytes_pending());

    if (n < 0) {
        state_ = Error;
        return;
    }

    writer_.advance_read(n);
}

bool Channel::closed() const {
    return state_ == Closed;
}

}

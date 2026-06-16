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

    switch (event) {
        case io::Readable: case io::Hup:
            handler_.on_readable(reader_, *this); break;
        case io::Writable:
            handler_.on_writable(writer_, *this); break;
        case io::RHup:
            state_ = Closing; break;
        case io::Error:
            state_ = Closing; break;
        default: break;
    }

    handler_.sync();
}

Channel::Stream Channel::stream() const { return stream_;}
Channel::State Channel::state() const { return state_; };

void Channel::shutdown() {
    state_ = Closed;
}

void Channel::mark_closing() {
    if (state_ == Closed) return;
    
    state_ = Closing;
}

void Channel::read() {
    
    ssize_t n = ::read(fd(), reader_.write_ptr(), reader_.capacity());

    if (n < 0) {
        state_ = Closing;
    }

    if (n == 0) state_ = Closing;
    
    reader_.update(n);
}

void Channel::write() {
    ssize_t n = ::write(fd(), writer_.read_ptr(), writer_.bytes_pending());

    if (n < 0) {
        handler_.close_channel(*this);
        state_ = Closing;
        return;
    }

    writer_.advance_read(n);
}

}

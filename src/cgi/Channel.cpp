
#include "Channel.hpp"
#include "http/pipeline/handlers/CgiHandler.hpp"

namespace cgi {

Channel::~Channel() {}

std::string channel_type(Channel::Stream s) {

    if (s == Channel::Stdin) {
        return "Stdin";
    }

    if (s == Channel::Stdout) return "Stdout";

    return "Stderr";
}

void Channel::on_event(io::Event event) {

    size_t w = 0;
    std::string s = channel_type(stream_);
    switch (event) {
        case io::Readable: case io::Hup:
            std::cout << "Channel Readable: " << s << "\n";
            w = handler_.on_readable(*this);
            buf.advance_read(w);
            break;
        case io::Writable:
            std::cout << "Channel Writable" << s <<"\n";
            w = handler_.on_writable(buf, *this);
            break;
        case io::RHup:
            std::cout << "Channel ReadEnd hangup\n";
            state_ = Closing; break;
        case io::Error:
            std::cout << "Channel Error\n";
            state_ = Closing; break;
        default: break;
    }
    
}

Channel::Stream Channel::stream() const { return stream_; }

bool Channel::closed() const { return state_ == Closed; }

Channel::State Channel::state() const { return state_; }

BufferView Channel::view() const {
    return BufferView(buf.read_ptr(), buf.bytes_pending());
}

void Channel::shutdown() {
    state_ = Closed;
}

void Channel::mark_closing() {
    if (state_ == Closed) return;
    
    state_ = Closing;
}

void Channel::read() {
    
    buf.compact();

    ssize_t n = ::read(fd(), buf.write_ptr(), buf.bytes_free());

    if (n < 0) {
        state_ = Error;
        return;
    }
    
    buf.advance_write(n);
}

void Channel::write() {
    ssize_t n = ::write(fd(), buf.read_ptr(), buf.bytes_pending());

    if (n < 0) {
        state_ = Closing;
        return;
    }

    if (n == 0) state_ = Closing;

    buf.advance_read(n);
    buf.compact();
}

}

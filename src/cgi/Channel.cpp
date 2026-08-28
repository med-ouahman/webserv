
#include "Channel.hpp"
#include "http/pipeline/handlers/CgiHandler.hpp"

#include <cerrno>

namespace cgi {

Channel::~Channel() {}

void Channel::on_event(io::Event event) {
	size_t consumed = 0;

	if (stream_ == Stdin) {
		if (event & io::Error) {
			state_ = Error;
			return;
		}
		if (event & (io::Hup | io::RHup)) {
			state_ = Closing;
			return;
		}
		if (event & io::Writable)
			handler_.on_writable(buf, *this);
		return;
	}

	if (event & (io::Readable | io::Hup)) {
		consumed = handler_.on_readable(*this);
		buf.advance_read(consumed);
	}

	if (state_ == Closed)
		return;
	if (event & io::Error)
		state_ = Error;
	else if (event & io::RHup)
		state_ = Closing;
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
	if (buf.bytes_free() == 0)
		return;

    ssize_t n = ::read(fd(), buf.write_ptr(), buf.bytes_free());

    if (n < 0) {
        state_ = Error;
        return;
    }
	if (n == 0) {
		state_ = Closing;
		return;
	}

    buf.advance_write(n);
}

void Channel::write() {
	if (buf.bytes_pending() == 0)
		return;

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

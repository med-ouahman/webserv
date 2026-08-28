
#include "Connection.hpp"
#include "Context.hpp"
#include <iostream>
#include "Server.hpp"
#include <sys/socket.h>

namespace net {

Connection::Connection(UniqueFd& uniq,
    io::Event events,
    const std::vector<const config::ServerConfig*>& servers,
    RuntimeServices& services)
    : AEventHandler(uniq.release(), events),
    state_(Reading),
    ctx(servers, fd(), fd(), services),
    reader_(in),
    writer_(out) {
}

Connection::~Connection() {

}

bool Connection::closing() const { return state_ == Closing; }

void Connection::update(http::ContextAction action) {

    if (action == http::AC_CLOSE && !writer_.empty()) return ;

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
        case Writing: new_events = (io::Event)(io::Readable | io::Writable); break;
        case Closing: new_events = io::Close; break;
    }

    if (new_events != events()) update_events(new_events);
}

void Connection::sync() {
    
    ctx.timeout();

	if (ctx.nextAction() == http::AC_NONE) ctx.process();
    
    update(ctx.nextAction());
}

void Connection::on_event(io::Event events) {
	
	if (events & (io::Hup | io::RHup | io::Error)) {
        state_ = Closing;
        return;
	}

	if (events & io::Writable) on_writable();

	if (events & io::Readable) on_readable();
}

void Connection::on_readable() {
    if (state_ == Closing) return;

    read();

    BufferView view(reader_.read_ptr(), reader_.bytes_pending());
    size_t n = ctx.consume(view);
    reader_.advance_read(n);
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

    if (n <  0) {
        state_ = Closing;
        return;
    }

    writer_.advance_read(n);
    writer_.compact();
}

void Connection::on_writable() {
    if (state_ == Closing) return;

	usize produced = ctx.produce(writer_.write_ptr(), writer_.bytes_free());
	writer_.advance_write(produced);
	write();

	if (writer_.empty()
		&& ctx.nextAction() == http::AC_WRITE
		&& produced == 0)
		ctx.advanceCycle();
}

}

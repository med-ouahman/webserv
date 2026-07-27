
#include "Connection.hpp"
#include "http/Context.hpp"
#include <iostream>
#include "Server.hpp"
#include <sys/socket.h>
#include <sstream>
#include <fcntl.h>

namespace net {

Connection::Connection(UniqueFd& uniq, io::Event events, RuntimeServices& services, const ConnectionInfo& info)
    : AEventHandler(uniq.release(), events),
    state_(Reading),
    close_after_write(false),
    last_activity_(),
    lifetime_(),
    ctx(info.servers, fd(), fd(), services),
    reader_(in),
    writer_(out),
    info_(info) {
}

Connection::~Connection() {

    std::stringstream ss;

    ss << "Connection closed FD (" << fd() << ")"; 
    Server::logger.log(logger::Info, ss.str());
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
        case Writing: new_events = (io::Event)(io::Writable); break;
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
    read();
    if (state_ == Closing) return;
    size_t n = ctx.consume(reader_.read_ptr(), reader_.bytes_pending());    
    reader_.advance_read(n);
}

void Connection::read() {

    if (state_ == Closing) return;
    reader_.compact();
    ssize_t n = ::recv(fd(), reader_.write_ptr(), reader_.bytes_free(), 0);

    if (n <= 0)
    {
        std::stringstream ss;
        ss << "connection closed by client FD (" << fd() << ")";
        Server::logger.log(logger::Info, ss.str());
        state_ = Closing;
        return;
    }

    std::stringstream ss; 
    ss << "Connection received " << n << " bytes";
    Server::logger.log(logger::Info, ss.str());
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

    size_t m = ctx.produce(writer_.write_ptr(), writer_.bytes_free());
    writer_.advance_write(m);
    write();

	if (writer_.empty() && ctx.nextAction() == http::AC_WRITE)
		ctx.advanceCycle();
}

}

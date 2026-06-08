#include "Connection.hpp"
#include "Context.hpp"

#include "CGIRequestHandler.hpp"
#include "Dispatcher.hpp"

namespace net {

Connection::Connection(int _fd, io::Event events, RegisterContext& regis_ctx)
    : stream_(_fd, events, *this),
    state_(READING),
    close_after_write(false),
    last_activity_(),
    lifetime_(),
    register_ctx(regis_ctx),
    ctx() {}

Connection::~Connection() {
    /* cp */
}

void Connection::consume(BufferReader& view) {
    std::cout << view.str();
    state_ = WRITING;
}

void Connection::produce(BufferWriter& writer) {
    http::Request r;
    http::ResolutionResult rs;
    http::CGIOutputContext c;
    c.ctx = this;
    c.cb_ = on_cgi;
    http::CGIControl ctl(register_ctx, c);
    http::CGIRequestHandler* h = new http::CGIRequestHandler(rs, r, ctl);
}

ConnectionState Connection::state() const {
    return state_;
}

void Connection::on_cgi(void* ctx, http::CGIResult const& r) {
net::Connection* c = static_cast<Connection*>(ctx);
c->on_cgi_data(r);
}

void Connection::on_cgi_data(http::CGIResult const& r) {

    std::cout << int(r.code);
    
}

}

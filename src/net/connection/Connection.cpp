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
    ctx() {h=NULL;}

Connection::~Connection() {
    /* cp */
}

void Connection::consume(BufferReader& reader) {
    std::cout << reader.data(), reader.size();
    http::Request r;
    http::ResolutionResult rs;
    http::CGIOutputContext c;
    c.ctx = this;
    c.cb_ = on_cgi;
    http::CGIControl ctl(register_ctx, c);

    if (!h)h = new http::CGIRequestHandler(rs, r, ctl);
    h->handle();
}

void Connection::produce(BufferWriter& writer) {

    (void)writer;
    state_ = CLOSING;
}

ConnectionState Connection::state() const {
    return state_;
}

void Connection::on_cgi(void* ctx, http::CGIResult const& r) {
    net::Connection* c = static_cast<Connection*>(ctx);
    c->on_cgi_data(r);
}

void Connection::on_cgi_data(http::CGIResult const& r) {
    std::cout << "r.code: " << r.code << '\n';;;;;
    if (!h)return;
    if (h->done()) {
        delete h;
        h = NULL;
        state_ = WRITING;
        close_after_write = true;
    }    
}

}

#include "Connection.hpp"
#include <unistd.h>
#include <cstring>
#include <cerrno>

namespace net { 
    
Connection::Connection( int _fd, io::Event mask, ServerContext ctx, DisconnectCallback cb_ )
    : stream_(_fd, mask, *this),
    state(READING),
    server(ctx),
    disconnect_(cb_),
    close_after_write(false),
    last_activity_(),
    lifetime_()
  /*  ctx() */{}

Connection::~Connection() {}

void Connection::update_stream() {
    switch (state) {
        case READING:
            stream_.update_mask(io::READABLE);
            break;
        case WRITING:
            stream_.update_mask(io::WRITABLE);
            break;
        case CLOSING:
            stream_.update_mask(io::NONE);
            break;
        default:
            break;
    }
}

bool Connection::timedout() {

    /* in development */
    
    /*
    switch (ctx.state()) {
        case http::REQUEST_LINE:
        // timeout logic here      
        default:
        return false;
    }
    */

    return false;
}

void Connection::consume( DataView& view ) {
    /* in development */
    // ctx.consume(view.data(), view.size());
    std::cout << view.data(), view.size();
}

void Connection::produce( BufferWriter& writer ) {
    Base::io::Writer w(writer.data(), writer.size());
    /* in development */
    // ctx.produce(w);
    w.write("Hello from the client\n");
    std::cout << writer.data(), writer.size();
}

void Connection::on_stream_error() {
    state = CLOSING;
}

void Connection::on_stream_closed() {
    state = CLOSING;
}

io::Stream& Connection::stream() {
    return stream_;
}

}

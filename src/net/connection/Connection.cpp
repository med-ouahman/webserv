#include "Connection.hpp"
#include <unistd.h>
#include <cstring>
#include <cerrno>

#include <fcntl.h>
#include <sstream>

namespace net { 
    
Connection::Connection( int _fd, io::Event events )
    : stream_(_fd, events, *this),
    state_(READING),
    close_after_write(false),
    last_activity_(),
    lifetime_()
  /*  ctx() */{}

Connection::~Connection() {}

void Connection::update_stream() {
    switch (state_) {
        case READING:
            stream_.update_events(io::READABLE);
            break;
        case WRITING:
            stream_.update_events(io::WRITABLE);
            break;
        case CLOSING:
            stream_.update_events(io::NONE);
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
    state_ = WRITING;
    update_stream();
}

void Connection::produce( BufferWriter& writer ) {
    // Base::io::Writer w(writer.data(), writer.size());
    /* in development */
    // ctx.produce(w);
    if (writer.remaining() == 0 && close_after_write) {
        state_ = CLOSING;
        update_stream();
        return ;
    }

    std::cout << "Writing...\n";
    int fd = open("var/www/error_pages/404.html", O_RDONLY);
    char a[1024];
    int x;
    std::string h = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Length: ";
    std::string body;
    size_t si = 0;
    while ((x=read(fd,a,1024)) > 0) {
        si+=x;
        body.append(a, x);
    }
    std::stringstream ss;
    ss << si;
    h += ss.str() + "\r\n\r\n";
    h.append(body);
    writer.write(h.c_str(), h.size());
    state_ = WRITING;
    close_after_write = true;
}

void Connection::on_stream_error() {
    state_ = CLOSING;
}

void Connection::on_stream_closed() {
    state_ = CLOSING;
}

io::Stream& Connection::stream() {
    return stream_;
}

ConnectionState Connection::state() const {
    return state_;
}

}


#include "Connection.hpp"

namespace net {

void Connection::on_event(io::Event events) {
   
    switch (events) {
        case io::Readable:
            std::cout << "Connection Readable\n";
            on_readable();
            break;
        case io::Writable:
            std::cout << "Connection Writable\n";
            on_writable();
            break;
        case io::Hup: case io::RHup:
            std::cout << "Connection Hangup\n";
            state_ = Closing;
            break;
        case io::Error:
            std::cout << "Channel Error\n";
            /* need log */
            state_ = Closing;
            break;
        default:
            break;
    }
    
}

void Connection::on_readable() {
    read();
    
    if (state_ == Closing) return;

    ctx.consume(rcvbuf.data_, rcvbuf.size_);
}

void Connection::on_writable() {

    if (state_ == Closing) return;

    size_t n   = ctx.produce(sndbuf.data_+sndbuf.offset_, sndbuf.capacity_ - sndbuf.size_);

    sndbuf.size_ += n;
    
    write();
}

}

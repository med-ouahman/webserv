
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

    ctx.consume(reader_.data(), reader_.size());
}

void Connection::on_writable() {

    if (state_ == Closing) return;

    http::Error err = ctx.produce(writer_);

    if (err != http::ERR_NONE || writer_.size() == 0) return;
    
    write();
}

}


#include "Connection.hpp"

namespace net {

void Connection::on_event(io::Event events) {
   
    switch (events) {
        case io::Readable:
            on_readable();
            break;
        case io::Writable:
            on_writable();
            break;
        case io::Hup: case io::RHup:
            state_ = Closing;
            break;
        case io::Error:
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

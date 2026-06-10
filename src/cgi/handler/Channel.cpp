#include "Channel.hpp"
#include "CGIRequestHandler.hpp"

namespace http {

Channel::Channel(Stream s, int fd, io::Event events, CGIRequestHandler& h)
  : AEventHandler(fd, events),
    stream_(s),
    state_(Open),
    handler_(h),
    reader_(ReaderSize),
    writer_(WriterSize) {

}


Channel::~Channel() {

}

void Channel::on_event(io::Event event) {

    if (handler_.state() == CGIRequestHandler::Error) {
        state_ = Closed;
    }

    if (state_ == Closed || state_ == Error) return;

    switch (event) {
        case io::Readable:
            handler_.on_readable(*this);
            break;
        case io::Writable:
            handler_.on_writable(*this);
            break;
        case io::Hup: case io::RHup:
            handler_.on_ch_closed(*this);
            break;
        case io::ERROR:
            handler_.on_ch_error(*this);
    }
}

Channel::Stream Channel::stream() const {
    return stream_;
}


}

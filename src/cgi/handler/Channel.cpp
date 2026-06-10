#include "Channel.hpp"

namespace http {


Channel::Channel(int fd, io::Event events, CGIRequestHandler& h)
  : AEventHandler(fd, events),
    handler_(h),
    reader_(ReaderSize),
    writer_(WriterSize) {

}


Channel::~Channel() {

}

void Channel::on_event(io::Event event) {
	
}

}

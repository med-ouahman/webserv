
#include "IOChannel.hpp"
#include "CGIHandler.hpp"

namespace http {

    IOChannel::IOChannel( int fd_, CGIHandler* h, STDStream::Type stream_type, uint32_t event_mask )
        : Stream(fd_),
        listener(h),
        stream(stream_type),
        event(event_mask) {}

    IOChannel::~IOChannel() {}


    int IOChannel::get_fd( void ) const {
        return fd;
    }

    uint32_t IOChannel::get_event( void ) const {
        return event;
    }

    bool IOChannel::process( void ) {
    
        return true;
    }

    bool IOChannel::readbuf_drained() {
        return true;
    }
    
}

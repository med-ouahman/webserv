
#include "IOChannel.hpp"
#include "CGIHandler.hpp"

namespace http {

    IOChannel::IOChannel( int fd_, const CGIHandler* h, Stream::Type stream_type, uint32_t event_mask )
        : fd(fd_),
        cgi_handler(h),
        stream(stream_type),
        event(event_mask) {}

    IOChannel::~IOChannel() {
        ::close(fd);
        fd = -1;
    }

    void IOChannel::on_event( io::EventType type ) {
        /* map event to stream type */
    }

    int IOChannel::get_fd( void ) const {
        return fd;
    }

    uint32_t IOChannel::get_event( void ) const {
        return event;
    }
}

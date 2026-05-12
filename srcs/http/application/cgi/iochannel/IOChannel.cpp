
#include "IOChannel.hpp"
#include "CGIHandler.hpp"

namespace http {

    IOChannel::IOChannel( int fd_, CGIHandler& h, STDStream::Type stream_type, uint32_t event_mask )
        : Stream(fd_),
        cgi_handler(h),
        stream(stream_type),
        event(event_mask),
        state(IOChannelState::IDLE) {}

    IOChannel::~IOChannel() {}


    int IOChannel::get_fd() const {
        return fd;
    }

    uint32_t IOChannel::get_event() const {
        return event;
    }

    bool IOChannel::readbuf_drained() {
        return data_view.empty();
    }

    core::DataView& IOChannel::get_view( ) {
        return data_view;
    }

    void IOChannel::shutdown() {
        state = IOChannelState::CLOSED;
    }
    
}

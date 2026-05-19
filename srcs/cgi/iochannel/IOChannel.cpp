
#include "IOChannel.hpp"
#include "CGIRequestHandler.hpp"

namespace http {

    IOChannel::IOChannel( int fd_, CGIRequestHandler& h, STDStream::Type stream_type, uint32_t event_mask )
        : Stream(fd_),
        cgi_handler(h),
        stream(stream_type),
        event(event_mask),
        state(IOChannelState::IDLE) {}

    IOChannel::~IOChannel() {}

    uint32_t IOChannel::get_event() const {
        return event;
    }

    bool IOChannel::readbuf_drained() {
        return data_view.empty();
    }

    DataView& IOChannel::get_view( ) {
        return data_view;
    }

    void IOChannel::shutdown() {
        state = IOChannelState::CLOSED;
    }
    
}

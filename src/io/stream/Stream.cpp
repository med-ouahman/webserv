#include "Stream.hpp"

namespace io {

Stream::Stream(int fd, Event mask, IStreamDelegate& d)
    : AEventHandler(fd, mask),
    delegate(d),
    reader_(READ_BUFFER_SIZE),
    writer(WRITE_BUFFER_SIZE) {}

Stream::~Stream() {}

void Stream::on_event(io::Event event) {
    switch (event) {
        case READABLE:
            on_readable();
            break;
        case WRITABLE:
            on_writeable();
            break;
        case HUP: case RHUP:
            std::cout << "stream closed\n";
            delegate.on_stream_closed();
            break;
        case ERROR:
            std::cout << "errror\n";
            delegate.on_stream_error();
            break;
        default:
            break;
    }
}

void Stream::on_readable() {
    std::cout << "stream readable\n";
    ssize_t n = ::read(fd(), reader_.data(), reader_.capacity());
    if (n == 0) {
        std::cout << "Stream closed\n";
        delegate.on_stream_closed();
        return;
    }
    
    if (n < 0) {
         LOG_ERROR(MAKE_ERRNO_ERROR("Sream::on_readable()"));
        delegate.on_stream_error();
        return;
    }
    
    delegate.consume(reader_);
}

void Stream::on_writeable() {
    std::cout << "writing...\n";
    delegate.produce(writer);

    if (writer.length() == 0) return;
    
    ssize_t n = ::write(fd(), writer.read_ptr(), writer.bytes_pending());
    
    
    if (n < 0) {
        LOG_ERROR(MAKE_ERRNO_ERROR("Sream::on_writeable()"));
        delegate.on_stream_error();
        return;
    }
    std::cout << "Written: " << n << "\n";
    writer.advance_read(n);
}

void Stream::pause() {
    ctl_.paused_ = true;
    ctl_.saved_events = events();
    update_events(io::NONE);
}

void Stream::resume() {
    ctl_.paused_ = false;
    update_events(ctl_.saved_events);
    ctl_.saved_events = io::NONE;
}

}

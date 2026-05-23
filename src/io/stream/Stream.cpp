#include "Stream.hpp"

namespace io {
    
    Stream::Stream( int fd, EventMask mask )
        : AEventHandler(fd, mask),
        readbuf(),
        writer() {}

    Stream::~Stream() {}


    void Stream::on_event( EventType event ) {
        
        switch (event) {
            case READABLE:
                on_readable();
                break;
            case WRITABLE:
                on_writeable();
                break;
            case HUP: case RHUP:
                delegate->on_stream_closed();
                break;
            case ERROR:
                delegate->on_stream_error();
                break;
            default:
                break;
        }
        
    }

    void Stream::on_readable() {
    
        ssize_t n = ::read(fd(), readbuf, READ_BUFFER_SIZE);

        if (n == 0) {
            delegate->on_stream_closed();
            return ;
        }
        
        if (n < 0) {
            delegate->on_stream_error();
            return ;
        }
        
        DataView view(readbuf, n);

        delegate->consume(view);
    }

    void Stream::on_writeable() {

        if (writer.remaining() == 0) delegate->produce(writer);
        
        if (writer.size() == 0) return ;

        ssize_t n = ::write(fd(), writer.data(), writer.remaining());

        if (n < 0) {
            delegate->on_stream_error();
            return ;
        }

        writer.advance(n);   
    }
}
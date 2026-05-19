#include "Stream.hpp"

namespace io {
    
    Stream::Stream( int fd )
        : AEventHandler(fd),
        readbuf(),
        writebuff(),
        bytes_r(0),
        data_view(readbuf),
        writer(writebuff, WRITE_BUFFER_SIZE) {}


    void Stream::on_event( EventType event ) {
        
        io_event = event;
        handle_event();
        process();
    }

    void Stream::on_readable() {
    
        data_view.reset();
        read();
        
        if (bytes_r == 0) {
            on_read_eof();
            return ;
        }
        
        if (bytes_r < 0) {
            on_read_error();
            return ;
        }
        
        data_view.update(bytes_r);
        process_incoming_data();
    }

    void Stream::on_writeable() {

        process_outgoing_data();
        
        if (writer.size() == 0) {
            on_write_complete();
            return ;
        }


        write();
        if (bytes_r < 0) {
            on_write_error();
            return ;
        }

        writer.advance(bytes_r);
        
    }

    void Stream::read() {
        bytes_r = ::read(fd_, readbuf, READ_BUFFER_SIZE);
    }
    
    void Stream::write() {
        bytes_r = ::write(fd_, writer.data(), writer.remaining());
    }
}
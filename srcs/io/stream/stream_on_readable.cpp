#include "Stream.hpp"

namespace io {

    void Stream::on_readable() {
        
        if (readbuf_drained()) {

            read();
            
            if (bytes_r == 0) {
                on_read_eof();
                return ;
            }
            
            if (bytes_r < 0) {
                processing = false;
                return ;
            }
            
            data_view.data_ptr_ = readbuf;
            data_view.bytes_consumed = 0;
            data_view.len_ = bytes_r;
        }

        process_incoming_data();
    }
}


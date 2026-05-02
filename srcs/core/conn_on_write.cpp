
#include "Connection.hpp"
#include <cerrno>
#include <cerrno>
#include <string.h>

namespace core {

    void Connection::on_readable() {

        if (readbuf_drained()) {

            read();
            
            if (bytes_r <= 0) {
                if (bytes_r == 0) {
                    state = ConnectionState::CLOSING;
                }
                processing = false;
                return ;
            }
            view.data_ptr_ = readbuf;
            view.bytes_consumed = 0;
            view.len_ = bytes_r;
            bytes_received = bytes_r;
        }

        processing = process_incoming_data();
    }

    void Connection::on_writeable() {
        
        process_outgoing_data();
        
        if (sent_offset < bytes_to_write) {
            
            write();
            if (bytes_r < 0) {
                processing = false;
                return ;
            }

            sent_offset += bytes_r;
        }
    }
}

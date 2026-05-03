#include "Stream.hpp"

namespace io {

    void Stream::on_writeable() {

        process_outgoing_data();
        
        if (bytes_to_write == 0) {
            on_write_complete();
            return ;
        }
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

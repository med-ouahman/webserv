#include "Stream.hpp"

namespace io {

    void Stream::on_writeable() {

        process_outgoing_data();
        
        if (writer.size() == 0) {
            on_write_complete();
            return ;
        }

        if (writer.offset() < writer.size()) {
            
            write();
            if (bytes_r < 0) {
                on_write_error();
                return ;
            }

            writer.advance(bytes_r);
        }
    }
}

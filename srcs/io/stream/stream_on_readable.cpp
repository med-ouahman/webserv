#include "Stream.hpp"

namespace io {

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
}

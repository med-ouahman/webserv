#include "IOChannel.hpp"

namespace http {
    
    void IOChannel::process_incoming_data() {
        
        ScanResult r = listener->on_input_ready();
        
        if (ERROR == r || SUCCESS == r) {
            processing = false;
        }
    }

    void IOChannel::process_outgoing_data() {
        
        if (writer.offset() < writer.size())
            return ;

        ssize_t produced = listener->produce_output(writer.data(), writer.capacity());
        
        if (produced < 0 || produced == 0) {
            processing = false;
            return ;
        }

        writer.update(produced);
    }

    void IOChannel::on_read_eof() {

        listener->on_channel_closed();
        processing = false;
    }

    void IOChannel::on_write_complete() {
    
    }

    void IOChannel::on_write_error() {
        // 
    }

    void IOChannel::on_read_error() {
        processing = false;
        listener->on_ch_error();
    }

}

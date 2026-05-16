#include "IOChannel.hpp"
#include "CGIHandler.hpp"

namespace http {
    
    void IOChannel::process_incoming_data() {
        
        ScanResult r = cgi_handler.on_input_ready();
        
        if (ERROR == r || SUCCESS == r) {
            state = IOChannelState::CLOSED;
        }
    }

    void IOChannel::process_outgoing_data() {
        
        if (writer.offset() < writer.size())
            return ;

        ssize_t produced = cgi_handler.produce_output(&writer);
        
        if (produced < 0 || produced == 0) {
            return ;
        }

        writer.update(produced);
    }

    void IOChannel::on_read_eof() {
        
        cgi_handler.on_channel_closed();
    }

    void IOChannel::on_write_complete() {
        shutdown();
    }

    void IOChannel::on_write_error() {
        cgi_handler.on_ch_error();
    }

    void IOChannel::on_read_error() {
        cgi_handler.on_ch_error();
    }

}

#include "IOChannel.hpp"

namespace http {
    
    void IOChannel::process_incoming_data() {
        
        listener->on_input_ready(&data_view);
        processing = false;
    }

    void IOChannel::process_outgoing_data() {
        
        if (sent_offset < bytes_to_write)
            return ;

        sent_offset = 0;
        bytes_to_write = 0;
        ssize_t produced = listener->produce_output(writebuff, SEND_CHUNK_SIZE);
        
        if (produced < 0 || produced == 0) {
            processing = false;
            return ;
        }

        bytes_to_write = produced;
    }

    void IOChannel::on_read_eof() {
        std::cout << "CHannel done fr\n";
        listener->on_channel_closed();
        processing = false;
    }

    void IOChannel::on_write_complete() {
        ::close(fd);
    }

    void IOChannel::on_write_error() {
        // 
    }

}

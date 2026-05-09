#include "Connection.hpp"

namespace core {

    void Connection::process_outgoing_data() {

        if (writer.offset() < writer.size())
            return ;

        writer.reset();

        ssize_t produced = response.produce(&writer);
        
        if (produced < 0) {
            state = ConnectionState::CLOSING;
            processing = false;
            return ;
        }
        
        writer.advance(produced);

        if (writer.size() == 0) {
            if (!readbuf_drained()) state = ConnectionState::READING;
            else if (close_after_write) state = ConnectionState::CLOSING;
            else state = ConnectionState::IDLE;
        }
        
        processing = (state == ConnectionState::READING)
            || (state == ConnectionState::WRITING);
    }


    void Connection::on_write_complete() {
        /// nothing for now, the code above handles it??
    }

    void Connection::on_write_error() {
        processing = false;
    }

}

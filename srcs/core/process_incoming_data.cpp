
#include "Connection.hpp"
#include "LineScanner.hpp"

namespace core {
  
    void Connection::process_incoming_data() {

        http::ScanResult result;
        
        result = p.parse();

        if (p.get_parser_state() == http::ParseState::BODY) {
        
            body_p.detect_body_type(p.get_request().headers);
            result = body_p.parse_body();
        }
        
        switch (result) {
            case http::NEED_MORE:
                processing = true;
                break;
            case http::ERROR:
               on_client_error();
               processing = false;
               break;
            case http::SUCCESS:
                on_request_ready();
                processing = false;
            default:
                break;
        }
    }


    void Connection::process_outgoing_data() {

        if (writer.offset() < writer.size())
            return ;

        writer.reset();

        ssize_t produced = response.produce(&writer, writer.capacity());
        
        if (produced < 0) {
            processing = false;
            return ;
        }
        
        writer.update(produced);

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

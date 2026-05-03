
#include "Connection.hpp"
#include "LineScanner.hpp"

namespace core {
    
    /*
        !!!!! Too much for this function  ngl, try to deduce the imeprative lines
        make it more declarative, the assignments make the good look ugly and hard to read, it's like reading ASM
        this is C++
    */
  
    void Connection::process_incoming_data() {

        http::ScanResult result;
        
        p.set_data_view(&data_view);
        result = p.parse();

        if (p.get_parser_state() == http::ParseState::BODY) {
        
            body_p.detect_body_type(p.get_request().headers);
            body_p.set_data_view(&data_view);
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

        if (sent_offset < bytes_to_write)
            return ;
    
        bytes_to_write = 0;
        sent_offset = 0;

        ssize_t produced = response.produce(writebuff, SEND_CHUNK_SIZE);
        
        if (produced < 0) {
            processing = false;
            state = ConnectionState::CLOSING;
            return ;
        }
        
        bytes_to_write = produced;

        if (bytes_to_write == 0) {
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

}

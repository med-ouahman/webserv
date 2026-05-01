
#include "Connection.hpp"
#include "LineScanner.hpp"

namespace core {
    
    /*
        !!!!! Too much for this function  ngl, try to deduce the imeprative lines
        make it more declarative, the assignments make the good look ugly and hard to read, it's like reading ASM
        this is C++
    */
  
    bool Connection::process_incoming_data() {
    
        p.set_data_view(readbuf, bytes_received);

        http::ScanResult result = p.parse();
        
        switch (result) {
            case http::NEED_MORE:
                return true;
            case http::ERROR:
               on_client_error();
               return false;
               break;
            case http::SUCCESS:
                on_request_ready();
                return false;
            default:
                break;
        }

        return false;
    }


    void Connection::process_outgoing_data() {

        if (sent_offset < bytes_to_write)
            return ;
    
        bytes_to_write = 0;
        sent_offset = 0;

        ssize_t produced = response.produce(writebuff, SEND_CHUNK_SIZE);
        
        if (produced < 0 || (produced == 0 && close_after_write)) {
            processing = false;
            state = ConnectionState::CLOSING;
            return ;
        }
        
        bytes_to_write = produced;

        if (bytes_to_write == 0) {
            state = readbuf_drained() ?
                ConnectionState::IDLE
                : ConnectionState::READING;
        }
        
        processing = (state == ConnectionState::READING)
            || (state == ConnectionState::WRITING);
        
    }

}

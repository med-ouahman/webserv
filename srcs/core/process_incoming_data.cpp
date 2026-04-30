
#include "Connection.hpp"

typedef http::HTTPParser::ParseState::Type ParseState;
typedef http::HTTPParser::ParseResult::Type ParseResult;

namespace core {
    
    /*
        !!!!! Too much for this function  ngl, try to deduce the imeprative lines
        make it more declarative, the assignments make the good look ugly and hard to read, it's like reading ASM
        this is C++
    */
  
    bool Connection::process_incoming_data( void ) {
        
        
        p.feed(readbuf, bytes_received);

        ParseResult result = p.consume();
        
        if (result == http::HTTPParser::ParseResult::NEED_MORE_BYTES) {
            return true;
        } else if (result == http::HTTPParser::ParseResult::PARSE_ERROR) {
            dispatcher.build_error_response(http::BAD_REQUEST, "Bad request");
            state = ConnectionState::WRITING;
            close_after_write = true;
            return false;
        }
        
        /* Response logic... */
        ++num_requests;
        std::cout << "REQUESTS: " << num_requests << "\n";
        http::HTTPRequest req = p.get_request();
        p.reset();
        close_after_write = !req.want_keep_alive();
    
        http::HTTPDispatcher::HandlerResult res = dispatcher.handle_request(req);

        if (res.response_type == http::HTTPResponseType::CGI) {
            state = ConnectionState::CGI;
            enter_cgi(res.cgi_ctx);
        } else {
            state = ConnectionState::WRITING;
        }
        return false;
    }


    bool Connection::process_outgoing_data( void ) {

        if (sent_offset == bytes_to_write) {
    
            sent_offset = 0;
            ssize_t to_write = dispatcher.produce(writebuff, SEND_CHUNK_SIZE);
            
            if (to_write < 0 || (to_write == 0 && close_after_write)) {
                processing = false;
                state = ConnectionState::CLOSING;
                return false;
            }
            
            bytes_to_write = to_write;

            if (bytes_to_write == 0) {
                state = readbuf_drained() ? ConnectionState::IDLE: ConnectionState::READING;
            }
            
            processing = (state == ConnectionState::READING) || (state == ConnectionState::WRITING);
            return false;
        }
        return true;
    }

}

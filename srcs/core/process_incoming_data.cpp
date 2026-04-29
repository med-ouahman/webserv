
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
        
        /* Parsing logic */
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
        http::HTTPRequest req = p.get_request();
        p.reset();
        close_after_write = !req.want_keep_alive();
    
        http::HTTPDispatcher::HandlerResult res = dispatcher.handle_request(req);

        if (res.response_type == http::HTTPResponseType::CGI) {
            state = ConnectionState::CGI;
            enter_cgi(res.cgi_ctx);
        } else {
            std::cout << "writing\n";
            state = ConnectionState::WRITING;
        }

        return false;
    }


    bool Connection::process_outgoing_data( void ) {

        if (sent_offset == bytes_to_write) {
            
            sent_offset = 0;
            if (!advance()) {
                processing = false;
                return false;
            }

            if (bytes_to_write == 0) {
                if (close_after_write) {
                    state = ConnectionState::CLOSING;
                    processing = false;
                } else if (readbuf_drained()) {
                    state = ConnectionState::IDLE;
                    processing = false;
                } else {
                    std::cout << "Back to reading...\n";
                    state = ConnectionState::READING;
                    processing = true;
                }
            }

            return false;
        }

        return true;
    }

}


#include "Connection.hpp"

namespace core {
    
    bool Connection::on_bytes( void ) {
        if (ConnectionState::CLOSING == state) {
            return false;
        }
        
        if (p.get_bytes_consumed() == 0) {
            p.set_data_buff(read_buff, bytes_received);
        }

        state = ConnectionState::READING;
        while (true) {
            http::HTTPParser::ParseResult::Type result = p.consume();
            if (result == http::HTTPParser::ParseResult::NEED_MORE_BYTES) {
                bytes_received = 0;
                return true;
            } else if (result == http::HTTPParser::ParseResult::PARSE_ERROR) {
                handler.build_error_response(http::BAD_REQUEST, "Bad request");
                handler.serialize();
                state = ConnectionState::WRITING;
                bytes_received = 0;
                close_after_write = true;
                return false;
            } else if (result == http::HTTPParser::ParseResult::TIMEOUT) {
                bytes_received = 0;
                state = ConnectionState::CLOSING;
                return false;
            }
            ++num_requests;
            http::HTTPRequest req = p.get_request();
            p.reset();
            close_after_write = !req.want_keep_alive();
            handler.handle_request(req);
            close_after_write = !handler.allow_presistance(close_after_write);
            bytes_received -= p.get_bytes_consumed();
            state = ConnectionState::WRITING;
            return false;
        }
        return true;
    }
}

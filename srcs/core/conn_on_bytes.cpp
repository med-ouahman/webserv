
#include "Connection.hpp"
#include <cerrno>

namespace core {
    
    bool Connection::on_bytes( void ) {
        if (CLOSING == state) {
            return false;
        }

        if (p.get_bytes_consumed() == 0) {
            p.set_data_buff(read_buff, bytes_received);
        }
        state = READING;
        while (true) {
            http::HTTPParser::ParseResult::Type result = p.consume();
            if (result == http::HTTPParser::ParseResult::NEED_MORE_BYTES) {
                read_buff_drained = true;
                return true;
            } else if (result == http::HTTPParser::ParseResult::PARSE_ERROR) {
                handler.build_error_response(http::BAD_REQUEST, "Bad request");
                handler.serialize();
                state = WRITING;
                close_after_write = true;
                return false;
            } else if (result == http::HTTPParser::ParseResult::TIMEOUT) {
                std::cout << "Timeout??\n";
                state = CLOSING;
                return false;
            }
            ++num_requests;
            http::HTTPRequest req = p.get_request();
            p.reset();
            close_after_write = !req.want_keep_alive();
            handler.handle_request(req);
            close_after_write = !handler.allow_presistance(close_after_write);
            read_buff_drained = p.get_bytes_consumed() == bytes_received;
            state = WRITING;
            return false;
        }
        return true;
    }
}

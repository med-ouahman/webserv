
#include "Connection.hpp"

typedef http::HTTPParser::ParseState::Type ParseState;
typedef http::HTTPParser::ParseResult::Type ParseResult;

namespace core {
    bool Connection::process_incoming_data( void ) {        
        
        p.set_data_buff(read_buff, bytes_received);
        ParseResult result = p.consume();

        if (result == http::HTTPParser::ParseResult::NEED_MORE_BYTES) {
            return true;
        } else if (result == http::HTTPParser::ParseResult::PARSE_ERROR) {
            dispatcher.build_error_response(http::BAD_REQUEST, "Bad request");
            state = ConnectionState::WRITING;
            close_after_write = true;
            return false;
        }
        ++num_requests;
        http::HTTPRequest req = p.get_request();
        p.reset();
        close_after_write = !req.want_keep_alive();
        dispatcher.handle_request(req);
        state = ConnectionState::WRITING;
        return false;
    }
}

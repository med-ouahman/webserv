
#include "Connection.hpp"

namespace core {
    
    bool Connection::process_incoming_data( void ) {        
        
        p.set_data_buff(read_buff, bytes_received);

        http::HTTPParser::ParseState::Type prev_state = p.get_parser_state();
        http::HTTPParser::ParseResult::Type result = p.consume();
        http::HTTPParser::ParseState::Type parse_state = p.get_parser_state();

        if (parse_state == prev_state) {
            current_state_ticks++;
        }

        if (parse_state == http::HTTPParser::ParseState::REQUEST_LINE) {
            current_state_tick_limit = REQUEST_LINE_LIMIT_TICKS;

        } else if (parse_state == http::HTTPParser::ParseState::HEADERS) {
            current_state_tick_limit = HEADERS_LIMIT_TICKS;

        } else if (parse_state == http::HTTPParser::ParseState::BODY) {
            current_state_tick_limit = BODY_LIMIT_TICKS;
            bytes_parsed_since_progress += p.get_body_bytes_consumed();
            if (bytes_parsed_since_progress >= MIN_PROGRESS_BYTES) {
                bytes_parsed_since_progress = 0;
                current_state_ticks = 0;
            }
        }

        if (current_state_tick_limit < current_state_ticks) {
            state = ConnectionState::CLOSING;
            return false;
        }
        
        current_state_ticks = 0;

        if (result == http::HTTPParser::ParseResult::NEED_MORE_BYTES) {
            return true;
        } else if (result == http::HTTPParser::ParseResult::PARSE_ERROR) {
            handler.build_error_response(http::BAD_REQUEST, "Bad request");
            handler.serialize();
            state = ConnectionState::WRITING;
            close_after_write = true;
            return false;
        }
        ++num_requests;
        http::HTTPRequest req = p.get_request();
        p.reset();
        close_after_write = !req.want_keep_alive();
        handler.handle_request(req);
        state = ConnectionState::WRITING;
        return false;    
    }
}

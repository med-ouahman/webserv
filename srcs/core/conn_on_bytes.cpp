
#include "Connection.hpp"

namespace core {
    
    bool Connection::on_bytes( char* buff ) {
        
        while (true) {
            http::HTTPParser::ParseResult result = p.consume(buff);
            if (result == http::HTTPParser::CONTINUE) {
                continue;
            }
            if (result == http::HTTPParser::NEED_MORE_BYTES) {
                return true;
            } else if (result == http::HTTPParser::PARSE_ERROR) {

                handler.build_error_response(http::BAD_REQUEST, "Bad request");
                handler.serialize();
                state = WRITING;
                close_after_write = true;
                return false;
            }
            http::HTTPRequest req = p.get_request();
            keep_alive = req.want_keep_alive();
            p.reset();
            close_after_write = handler.handle_request(req);
            keep_alive = keep_alive && handler.allow_presistance();
            handler.serialize();
            state = WRITING;
            return false;
        }

        return true;
    }
}

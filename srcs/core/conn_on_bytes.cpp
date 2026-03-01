
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
                response_buff = handler.serialize();
                state = WRITING;
                close_after_write = true;
                return false;
            }

            http::HTTPRequest req = p.get_request();
            if (!req.version_supported()) {
                handler.build_error_response(http::VERSION_NOT_SUPPORTED, "Version not supported");
                response_buff = handler.serialize();
                state = WRITING;
                close_after_write = true;
                return false;
            }

            keep_alive = req.want_keep_alive();
            p.reset();
            handler.handle_request(req);
            response_buff = handler.serialize();
            state = WRITING;
            close_after_write = true;
            return false;
        }

        return true;
    }
}

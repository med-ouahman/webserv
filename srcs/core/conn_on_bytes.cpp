
#include "Connection.hpp"

namespace core {
    
    bool Connection::on_bytes( char* buff ) {
        // std::cout << "handling request: " << num_requests << '\n';
      
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
                num_requests++;
                state = WRITING;
                close_after_write = true;
                return false;
            }

            http::HTTPRequest req = p.get_request();

            p.reset();
            num_requests++;
            close_after_write = !req.want_keep_alive();
            handler.handle_request(req);
            close_after_write = !handler.allow_presistance(close_after_write);
            state = WRITING;
            return false;
        }

        return true;
    }
}

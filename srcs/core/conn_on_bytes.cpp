
#include "Connection.hpp"
#include <cerrno>

namespace core {
    
    bool Connection::on_bytes( const char* buff, ::ssize_t bytes ) {

        if (bytes <= 0) {
            if (bytes == 0 || errno != EAGAIN) {
                state = CLOSING;
            }
            return false;
        }
        
        while (true) {
            
            http::HTTPParser::ParseResult::Type result = p.consume(buff, bytes);
            
            if (result == http::HTTPParser::ParseResult::NEED_MORE_BYTES) {
                return true;
            } else if (result == http::HTTPParser::ParseResult::PARSE_ERROR) {
                handler.build_error_response(http::BAD_REQUEST, "Bad request");
                handler.serialize();
                num_requests++;
                state = WRITING;
                close_after_write = true;
                return false;
            }
            
            http::HTTPRequest req = p.get_request();

            num_requests++;
            p.reset();
            close_after_write = !req.want_keep_alive();
            handler.handle_request(req);
            close_after_write = !handler.allow_presistance(close_after_write);
            state = WRITING;
            std::cout << "sending request...\n";
            return false;
        }

        return true;
    }
}


#include "Connection.hpp"

namespace core {
    
    bool Connection::on_bytes( const char* buff, ::size_t size ) {
        // std::cout << "handling request: " << num_requests << '\n';
        
        while (true) {
            http::HTTPParser::ParseResult result = p.consume(buff, size);
            
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
            
            std::cout << "connection fd:" << fd << "\nrequests managed: " << num_requests << '\n';
            http::HTTPRequest req = p.get_request();
            std::cout << "method: '" << req.get_method(req.method) << "'\nuri: '" << req.url << "'\nversion: '" << req.version << "'\n";
            num_requests++;
            p.reset();
            close_after_write = !req.want_keep_alive();
            handler.handle_request(req);
            close_after_write = !handler.allow_presistance(close_after_write);
            state = WRITING;
            return false;
        }

        return true;
    }
}

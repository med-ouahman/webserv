#include "Connection.hpp"

namespace core {

    void Connection::on_request_ready() {

        ++num_requests;
        http::HTTPRequest req = p.get_request();
        p.reset();
        close_after_write = !req.want_keep_alive();
    
        /* Notice: Pass the body handler if you want the body*/
        http::HTTPDispatcher::HandlerResult res = dispatcher.handle_request(req);
        
        if (res.response_type == http::HTTPResponseType::CGI) {
            state = ConnectionState::CGI;
            invoke_cgi(res.cgi_ctx);
            body_handler.detect_body_type(req.headers);
        } else {
            state = ConnectionState::WRITING;
        }
    }

    void Connection::on_client_error() {
        dispatcher.build_error_response(http::BAD_REQUEST, "Bad request");
        response.body = "HTTP/1.1 400 Bad Request\r\n\r\n";
        state = ConnectionState::WRITING;

        close_after_write = true;
    }
    
}

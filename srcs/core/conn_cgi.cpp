#include "Connection.hpp"
#include "EventLoop.hpp"

namespace core {

    void Connection::invoke_cgi( const http::CGIContext& context ) {
        
        try {
            cgi_handler = new http::CGIHandler(*this, loop);
            cgi_handler->spawn(context);
        } catch ( std::runtime_error& err ) {
            std::cout << err.what() << "\n";
            release_cgi_handler();
            http::HTTPDispatcher::build_error_response(http::INTERNAL_SERVER_ERROR, "Internal Server Error");
            state = ConnectionState::WRITING;
            close_after_write = true;
        }
    
        state = ConnectionState::CGI;
    }

    void Connection::release_cgi_handler() {
        std::cout << "Detaching the CGIHandler after finish, EventLoop in control now\n";
        loop.add_cgi_handler(cgi_handler);
        cgi_handler = NULL;
    }

}
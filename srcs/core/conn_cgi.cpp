#include "Connection.hpp"
#include "EventLoop.hpp"

namespace core {

    void Connection::enter_cgi( const http::CGIContext& cgi_ctx ) {
        
        try {
            cgi_handler = new http::CGIHandler(*this, loop);
            cgi_handler->spawn(cgi_ctx);
        } catch (std::runtime_error& err) {
            std::cerr << err.what() << "\n";
            cgi_detach();
            dispatcher.build_error_response(http::INTERNAL_SERVER_ERROR, "Internal Server Error");
            state = ConnectionState::WRITING;
            close_after_write = true;
        }

    }

    void Connection::cgi_detach() {
        std::cout << "Detaching the CGIHandler after finish, EventLoop in control now\n";
        loop.add_cgi_handler(cgi_handler);
        cgi_handler = NULL;
    }

}
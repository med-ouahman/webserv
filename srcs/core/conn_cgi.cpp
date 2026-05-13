#include "Connection.hpp"
#include "EventLoop.hpp"

namespace core {

    void Connection::release_cgi_handler() {
        std::cout << "Detaching the CGIHandler after finish, EventLoop in control now\n";
        loop.add_cgi_handler(static_cast<http::CGIHandler*>(request_handler));
        request_handler = NULL;
    }

}
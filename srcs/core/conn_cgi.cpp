#include "Connection.hpp"
#include "EventLoop.hpp"

namespace core {

    /*
        after the connection has configured CGI, it passes the handler to event loop to watch for timeout and delete if the script is finished.
    */

    void Connection::release_cgi_handler() {
        std::cout << "Detaching the CGIHandler after finish, EventLoop in control now\n";
        loop.add_cgi_handler(static_cast<http::CGIHandler*>(request_handler));
        request_handler = NULL;
    }

}
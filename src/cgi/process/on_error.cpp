#include "CGIRequestHandler.hpp"
#include "Connection.hpp"
#include "EventLoop.hpp"
#include <signal.h>
#include <csignal>
#include <sys/wait.h>

namespace http {
    
    void CGIRequestHandler::on_error() {
        
        std::cout << int(cgi_state) << "\n";
        std::cout << strerror(errno) << "\n";
        std::cout << "CGI ERROR\n";
        cgi_state = CGIState::ERROR;
        stdin_ch.shutdown();
        stderr_ch.shutdown();
        stdout_ch.shutdown();

        conn.on_cgi_error(BAD_GATEWAY);
    }

}
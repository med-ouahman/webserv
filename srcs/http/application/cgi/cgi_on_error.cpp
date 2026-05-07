#include "CGIHandler.hpp"
#include "Connection.hpp"
#include "EventLoop.hpp"
#include <signal.h>
#include <sys/wait.h>

namespace http {
    
    void CGIHandler::on_error() {
        
        cgi_state = CGIState::ERROR;

        conn.on_cgi_error(BAD_GATEWAY, "Bad Gateway");
    }

}
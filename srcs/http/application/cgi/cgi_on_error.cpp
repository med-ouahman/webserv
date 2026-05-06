#include "CGIHandler.hpp"
#include "Connection.hpp"
#include "EventLoop.hpp"
#include <signal.h>
#include <sys/wait.h>

namespace http {
    
    void CGIHandler::on_error() {
        
        cgi_state = CGIState::ERROR;

        if (output_state == CGIOutputState::WRITING_BODY) {
            /* send the last chunk and close */
            return ;
        }

        conn.on_cgi_error(BAD_GATEWAY, "Bad Gateway");
    }

}
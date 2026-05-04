#include "CGIHandler.hpp"
#include "Connection.hpp"
#include "EventLoop.hpp"
#include <signal.h>
#include <sys/wait.h>

namespace http {
    
    void CGIHandler::on_error() {
        cgi_state = ERROR;
    }

}
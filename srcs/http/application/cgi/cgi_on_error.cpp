#include "CGIHandler.hpp"
#include "Connection.hpp"
#include "EventLoop.hpp"
#include <signal.h>
#include <sys/wait.h>
namespace http {
    
    void CGIHandler::on_error() {
        cgi_state = CGIState::ERROR;
    }

    void CGIHandler::terminate_process() {
        ::kill(cgi_pid, SIGKILL);
        ::waitpid(cgi_pid, &cgi_status, WNOHANG);
    }
}
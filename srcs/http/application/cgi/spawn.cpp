#include "CGIHandler.hpp"
#include "CGIContext.hpp"
#include "EventLoop.hpp"
#include <sys/wait.h>

namespace http {

    void CGIHandler::spawn( const CGIContext& context ) {
        
        if (cgi_state != CGIState::SPAWN) {
            return ;
        }

        cgi_state = CGIState::ACTIVE;
        cgi_pid = ::fork();
        if (cgi_pid == 0) {

            ::dup2(pipe_guard.stdin_pipe[0], STDIN_FILENO);
            ::dup2(pipe_guard.stdout_pipe[1], STDOUT_FILENO);
            ::dup2(pipe_guard.stderr_pipe[1], STDERR_FILENO);
                        
    
            pipe_guard.close_pipes();
            char* argv[] = {
                (char*)context.interpreter_path.c_str(),
                (char *)context.script_filename.c_str(), NULL};

            ::execve(context.interpreter_path.c_str(), argv, __environ);
            LOG_ERROR(MAKE_ERRNO_ERROR("execve()"));
            ::exit(EXIT_FAILURE);
            throw std::runtime_error(strerror(errno));
        }

        CLOSE_FD(pipe_guard.stdin_pipe[0]);
        CLOSE_FD(pipe_guard.stdout_pipe[1]);
        CLOSE_FD(pipe_guard.stderr_pipe[1]);
        
        if (cgi_pid < 0) {
            throw std::runtime_error(strerror(errno));
        }

        loop.add_fd(stdout_ch.get_fd(), stdout_ch.get_event(), &stdout_ch);
        loop.add_fd(stderr_ch.get_fd(), stderr_ch.get_event(), &stderr_ch);
    }


}

#include "CGIHandler.hpp"
#include "CGIContext.hpp"
#include "EventLoop.hpp"
#include <sys/wait.h>

namespace http {

    void CGIHandler::spawn( const CGIContext& context ) {
        
        if (cgi_state != SPAWN) {
            return ;
        }

        cgi_state = ACTIVE;
        cgi_pid = ::fork();
        if (cgi_pid == 0) {

            ::dup2(pipe_guard.stdin_pipe[0], STDIN_FILENO);
            ::dup2(pipe_guard.stdout_pipe[1], STDOUT_FILENO);
            ::dup2(pipe_guard.stderr_pipe[1], STDERR_FILENO);
    
            pipe_guard.close_pipes();

            ::execve(context.interpreter_path.c_str(), NULL, __environ);
            ::perror("execve");
            ::exit(EXIT_FAILURE);
        }

        CLOSE_FD(pipe_guard.stdin_pipe[0]);
        CLOSE_FD(pipe_guard.stdout_pipe[1]);
        CLOSE_FD(pipe_guard.stderr_pipe[1]);
        
        if (cgi_pid < 0) {
            cgi_state = ERROR;
            return ;
        }

        loop.register_io_handler(&stdin_ch);
        loop.register_io_handler(&stdout_ch);
        loop.register_io_handler(&stderr_ch);
    }

    void CGIHandler::handle_event( io::EventType event, Stream::Type stream ) {
        if (event == io::EventType::ERROR) {
            cgi_state = ERROR;
            return ;
        }

    }
}

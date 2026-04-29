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
            char* argv[] = {NULL};
            ::execve(context.interpreter_path.c_str(), argv, __environ);
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
    }

    void CGIHandler::handle_event( io::EventType event, STDStream::Type stream ) {
        if (event == io::ERROR) {
            cgi_state = ERROR;
            stream = STDStream::STDIN;
            return ;
        }
        if (stream == STDStream::STDERR) {

        }
    }
}

#include "CGIHandler.hpp"
#include "CGIContext.hpp"
#include "EventLoop.hpp"
#include <sys/wait.h>

namespace http {

    char** build_cgi_env() {
        char** arr = new char*[2];
        arr[0] = const_cast<char*>("CONTENT_LENGTH=100");
        arr[1] = NULL;
        return arr;
    }

    void CGIHandler::spawn( const CGIContext& context ) {
        
        if (cgi_state != CGIState::SPAWN) {
            return ;
        }

        cgi_state = CGIState::ACTIVE;
        cgi_pid = ::fork();
        if (cgi_pid == 0) {

            int body_fd = open(context.temp_body_path.c_str(), O_RDONLY);
            if (body_fd < 0) exit(EXIT_FAILURE);
            ::dup2(body_fd, STDIN_FILENO);
            ::dup2(pipe_guard.stdout_pipe[1], STDOUT_FILENO);
            ::dup2(pipe_guard.stderr_pipe[1], STDERR_FILENO);
                        
    
            pipe_guard.close_pipes();
            char* argv[] = {
                (char*)context.interpreter_path.c_str(),
                (char *)context.script_filename.c_str(), NULL};

            ::execve(context.interpreter_path.c_str(), argv, build_cgi_env());
            LOG_ERROR(MAKE_ERRNO_ERROR("execve()"));
            ::exit(EXIT_FAILURE);
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

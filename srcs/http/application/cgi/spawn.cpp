#include "CGIHandler.hpp"
#include "CGIContext.hpp"
#include "EventLoop.hpp"
#include <sys/wait.h>
#include <fcntl.h>
#include <cstdlib>
#include "Timestamp.hpp"

namespace http {

    char**build_cgi_env(){
        int n=0;for(;__environ[n];n++);
        char**arr=new char*[n+2];
        int x=n+2;
        for (n=0;n<x;++n)arr[n]=__environ[n];
        arr[n]=const_cast<char*>("HTTP_CONTENT_LENGTH=100");
        arr[n+1]=0;
        return arr;
    }

    void CGIHandler::spawn( const io::EventLoop& loop ) {
        
        if (cgi_state != CGIState::SPAWN) {
            std::cout << "Already spawned\n";
            return ;
        }

        CGIContext context = http::HTTPDispatcher::resolve_cgi_context(result);
        cgi_state = CGIState::ACTIVE;
        start_time.update();
        cgi_pid = ::fork();
        if (cgi_pid == 0) {

            ::dup2(pipe_guard.stdout_pipe[1], STDOUT_FILENO);
            ::dup2(pipe_guard.stderr_pipe[1], STDERR_FILENO);
                        
            pipe_guard.close_pipes();
            char* const argv[] = {
               const_cast<char*>( context.interpreter_path.c_str()), 
               const_cast<char*>(context.script_filename.c_str()),
               NULL};
               
            ::execve(context.interpreter_path.c_str(), argv, __environ);
            LOG_ERROR(MAKE_ERRNO_ERROR("execve()"));
            ::exit(EXIT_FAILURE);
        }

        CLOSE_FD(pipe_guard.stdin_pipe[0]);
        CLOSE_FD(pipe_guard.stdout_pipe[1]);
        CLOSE_FD(pipe_guard.stderr_pipe[1]);
        
        if (cgi_pid < 0) {
            throw std::runtime_error(strerror(errno));
        }

        loop.add_fd(stdout_ch.fd(), stdout_ch.get_event(), &stdout_ch);
        loop.add_fd(stderr_ch.fd(), stderr_ch.get_event(), &stderr_ch);
    }

}

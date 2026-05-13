#include "CGIHandler.hpp"
#include "CGIContext.hpp"
#include "EventLoop.hpp"
#include <sys/wait.h>
#include <fcntl.h>
#include <cstdlib>

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

    void CGIHandler::spawn( const io::EventLoop& loop, const CGIContext& context ) {
        
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
                        
            chdir(context.working_directory.c_str());
            pipe_guard.close_pipes();
            char* argv[] = {
                const_cast<char*>("./app"), NULL};

            ::execve("./app", argv, build_cgi_env());
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

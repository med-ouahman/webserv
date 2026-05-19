#include "CGIProcess.hpp"

namespace cgi {

    CGIProcess::CGIProcess() {

    }

    CGIProcess::~CGIProcess() {

    }


    void CGIProcess::spawn( const CGIContext& ctx, const io::EventLoop loop ) {
        (void)loop;
        if (cgi_state != CGIState::SPAWN) {
            std::cout << "Already spawned\n";
            return ;
        }

        CGIContext context = http::Dispatcher::resolve_cgi_context(result);

        cgi_timeout_secs = context.timeout_seconds;
        
        cgi_state = CGIState::ACTIVE;
        start_time.update();
        pid = ::fork();
        if (pid == 0) {

            ::dup2(stdout_.fd(), STDOUT_FILENO);
            ::dup2(stderr_.fd(), STDERR_FILENO);
                        
            pipe_guard.close_pipes();
            char* const argv[] = {
               const_cast<char*>( context.interpreter_path.c_str()), 
               const_cast<char*>(context.script_filename.c_str()),
               NULL};
               
            ::execve(context.interpreter_path.c_str(), argv, build_cgi_env(context));
            LOG_ERROR(MAKE_ERRNO_ERROR("execve()"));
            ::exit(EXIT_FAILURE);
        }

        CLOSE_FD(pipe_guard.stdin_pipe[0]);
        CLOSE_FD(pipe_guard.stdout_pipe[1]);
        CLOSE_FD(pipe_guard.stderr_pipe[1]);
        
        if (pid < 0) {
            throw std::runtime_error(strerror(errno));
        }

        loop.add_fd(stdout_.fd(), stdout_.get_event(), &stdout_);
        loop.add_fd(stderr_.fd(), stderr_.get_event(), &stderr_);
    }

}
#include "CGIProcess.hpp"
#include <csignal>
#include <sys/wait.h>

namespace cgi {

    CGIProcess::CGIProcess( const CGIExecContext& ctx )
        : pid(-1),
        status(0x0),
        pipe_guard(),
        stdout_(pipe_guard.stdout_pipe[0], io::READABLE),
        stderr_(pipe_guard.stderr_pipe[0], io::READABLE),
        spawn_time(),
        sigterm_sent_at(0) {
        

        timeout_secs = ctx.timeout_seconds;
        spawn_time.update();

        pid = ::fork();
        if (pid == 0) {

            ::dup2(ctx.stdin_fd, STDIN_FILENO);
            ::dup2(stdout_.fd(), STDOUT_FILENO);
            ::dup2(stderr_.fd(), STDERR_FILENO);
            pipe_guard.close_pipes();

            char* const argv[] = {
               const_cast<char*>( ctx.interpreter_path.c_str()), 
               NULL };
            ::execve(ctx.interpreter_path.c_str(), argv, ctx.envp);
            LOG_ERROR(MAKE_ERRNO_ERROR("execve()"));
            ::exit(EXIT_FAILURE);
        }

        CLOSE_FD(pipe_guard.stdout_pipe[1]);
        CLOSE_FD(pipe_guard.stderr_pipe[1]);
        
        if (pid < 0) throw std::runtime_error(strerror(errno));
    }

    CGIProcess::~CGIProcess() {

        ::waitpid(pid, &status, 0);  // should be blocking to ensuer the process is reaped
        
    }

    void CGIProcess::on_stdout_readable() {
        
    }

    void CGIProcess::on_stdin_writeable() {

    }

    void CGIProcess::on_stderr_readable() {

    }

    void CGIProcess::on_error() {

    }

    io::Stream const& CGIProcess::stdout() const {
        return stdout_;
    }

    io::Stream const& CGIProcess::stderr() const {
        return stderr_;
    }


    bool CGIProcess::timedout() {
    
        if (sigterm_sent_at.seconds() == 0) {
            if (spawn_time.elapsed() >= timeout_secs) {
                sigterm_sent_at.update();
                ::kill(pid, SIGTERM);
                if (0 == ::waitpid(pid, &status, WNOHANG)) return false;
                return true;
            }
    }

    if (sigterm_sent_at.elapsed() >= 2) {
        ::kill(pid, SIGKILL);
    }

    return false;
}

}
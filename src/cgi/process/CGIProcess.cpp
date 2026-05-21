#include "CGIProcess.hpp"

namespace cgi {

    CGIProcess::CGIProcess( const CGIExecContext& ctx )
        : pid(-1),
        status(0x0),
        pipe_guard(),
        stdout_(pipe_guard.stdout_pipe[1], io::READABLE),
        stderr_(pipe_guard.stderr_pipe[1], io::READABLE),
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

        
    }

    void CGIProcess::on_stdout_readable() {
        
    }

    void CGIProcess::on_stdin_writeable() {

    }

    void CGIProcess::on_stderr_readable() {

    }

    void CGIProcess::on_error() {

    }

    io::Stream const& CGIProcess::stdin() const {

        return stdin_;
    }

    io::Stream const& CGIProcess::stdout() const {
        return stdout_;
    }

    io::Stream const& CGIProcess::stderr() const {
        return stderr_;
    }
}
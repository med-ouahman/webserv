#include "CGIProcess.hpp"
#include <csignal>
#include <sys/wait.h>

namespace cgi {


    CGIProcess::CGIProcess( const CGIExecContext& ctx )
        : pid(-1),
        status(0x0),
        stdout_set(),
        stderr_set(),
        stdout_(stdout_set.read_end.get(), io::READABLE),
        stderr_(stderr_set.read_end.get(), io::READABLE),
        spawn_time(),
        sigterm_sent_at(0) {
        
        if (!stderr_set || !stdout_set) {
            state = ERROR;
            return;
        }

        timeout_secs = ctx.timeout_seconds;
        spawn_time.update();

        pid = ::fork();
        if (pid == 0) {

            ::dup2(ctx.stdin_fd, STDIN_FILENO);
            ::dup2(stdout_set.write_end.get(), STDOUT_FILENO);
            ::dup2(stderr_set.write_end.get(), STDERR_FILENO);

            stdout_set.close();
            stderr_set.close();
            char* const argv[] = {
               const_cast<char*>( ctx.interpreter_path.c_str()), 
               NULL };
            ::execve(ctx.interpreter_path.c_str(), argv, ctx.envp);
            LOG_ERROR(MAKE_ERRNO_ERROR("execve()"));
            ::exit(EXIT_FAILURE);
        }

        stdout_set.write_end.reset();
        stderr_set.write_end.reset();
        
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
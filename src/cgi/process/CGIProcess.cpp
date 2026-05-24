#include "CGIProcess.hpp"
#include <csignal>
#include <sys/wait.h>

namespace cgi {

CGIProcess::CGIProcess( const CGIExecContext& ctx )
    : pid(-1),
    status(0x0),
    stdout_pipe_(),
    stderr_pipe_(),
    spawn_time(),
    sigterm_sent_at(0), state(SPAWN) {
    
    if (!stderr_pipe_ || !stdout_pipe_) {
        state = ERROR;
        return;
    }

    timeout_secs = ctx.timeout_seconds;
    spawn_time.update();
    pid = ::fork();
    if (pid == 0)
    {
        ::dup2(ctx.stdin_fd.fd(), STDIN_FILENO);
        ::dup2(stdout_pipe_.write_end().get(), STDOUT_FILENO);
        ::dup2(stderr_pipe_.write_end().get(), STDERR_FILENO);
        stdout_pipe_.close();
        stderr_pipe_.close();
        ::execve(ctx.interpreter_path.c_str(), ctx.argv.data(), ctx.envp.data());
        LOG_ERROR(MAKE_ERRNO_ERROR("execve()"));
        ::exit(EXIT_FAILURE);
    }

    stdout_pipe_.write_end().reset();
    stderr_pipe_.write_end().reset();
    
    if (pid < 0) state = ERROR;
}

CGIProcess::~CGIProcess() {
    ::waitpid(pid, &status, 0);  // should be blocking to ensuer the process is reaped   
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
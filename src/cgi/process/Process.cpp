#include "Process.hpp"
#include <csignal>
#include <sys/wait.h>
#include <cstdlib>

namespace cgi {

time_t Process::timeout_secs;

Process::Process( const resolver::CGIExecContext& ctx )
    : pid(-1),
    status(0x0),
    stdin_pipe_(),
    stdout_pipe_(),
    stderr_pipe_(),
    spawn_time(),
    sigterm_sent_at(0), state(SPAWN) {
    
    if (!stderr_pipe_ || !stdout_pipe_  || !stdin_pipe_) {
        state = ERROR;
        return;
    }

    timeout_secs = ctx.timeout_seconds;
    spawn_time.update();
    pid = ::fork();
    if (pid == 0)
    {
        ::dup2(ctx.stdin_fd.get(), STDIN_FILENO);
        ::dup2(stdout_pipe_.write_end().get(), STDOUT_FILENO);
        ::dup2(stderr_pipe_.write_end().get(), STDERR_FILENO);
        stdout_pipe_.close();
        stderr_pipe_.close();
        ::execve(ctx.interpreter.c_str(), ctx.argv.argv(), ctx.envp.argv());
        LOG_ERROR(MAKE_ERRNO_ERROR("execve()"));
        ::exit(EXIT_FAILURE);
    }

    stdin_pipe_.read_end().close();
    stdout_pipe_.write_end().close();
    stderr_pipe_.write_end().close();
    
    if (pid < 0) state = ERROR;
}

Process::~Process() {
    ::waitpid(pid, &status, 0);  // should be blocking to ensuer the process is reaped   
}

bool Process::timedout() {
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

Pipe& Process::stdin_pipe() {
    return stdin_pipe_;
}

Pipe& Process::stdout_pipe() {
    return stdout_pipe_;
}

Pipe& Process::stderr_pipe() {
    return stderr_pipe_;
}

}

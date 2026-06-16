#include "Process.hpp"
#include <csignal>
#include <sys/wait.h>
#include <cstdlib>
#include "EnvBuilder.hpp"
#include <iostream>
#include "Result.hpp"

namespace cgi {

time_t Process::timeout_secs;

Process::Process(const CGIExecContext& ctx)
    : state_(Spawn), 
    pid(-1),
    status(0),
    stdin_pipe_(),
    stdout_pipe_(),
    stderr_pipe_(),
    spawn_time(),
    sigterm_sent_at(0) {
    
    if (!stderr_pipe_ || !stdout_pipe_  || !stdin_pipe_) {
        state_ = Error;
        return;
    }

    state_ = Running;
    if (ctx.stdin_fd == STDIN_FILENO)
        stdin_pipe_.close_write_end();
    timeout_secs = ctx.timeout_seconds;

    spawn_time.update();
    pid = ::fork();
    if (pid == 0)
    {
        if (ctx.stdin_fd != STDIN_FILENO)
            ::dup2(ctx.stdin_fd, STDIN_FILENO);
        ::dup2(stdout_pipe_.write_end(), STDOUT_FILENO);
        ::dup2(stderr_pipe_.write_end(), STDERR_FILENO);
    
        stdin_pipe_.close();
        stdout_pipe_.close();
        stderr_pipe_.close();
        ::execve(ctx.argv.argv()[0], ctx.argv.argv(), ctx.envp.argv());
        LOG_ERROR(MAKE_ERRNO_ERROR("execve()"));
        ::exit(EXIT_FAILURE);
    }

    stdin_pipe_.close_read_end();
    stdout_pipe_.close_write_end();
    stderr_pipe_.close_write_end();
    
    
    if (pid < 0) {
        state_ = Error;
    }
}


Process::~Process() {
    ::waitpid(pid, &status, 0);
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

bool Process::running() const {
    return state_ == Running;
}

bool Process::want_stdin() {
    return stdin_pipe_.write_end() >= 0;
}

}

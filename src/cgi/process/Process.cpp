#include "Process.hpp"
#include <csignal>
#include <sys/wait.h>
#include <cstdlib>
#include "EnvBuilder.hpp"
#include <iostream>
#include "Result.hpp"

namespace cgi {

Process::Process(const CGIExecContext& ctx)
    : state_(Spawn), 
    pid_(-1),
    status_(0),
    stdin_pipe_(),
    stdout_pipe_(),
    stderr_pipe_() {
    
    if (!stderr_pipe_ || !stdout_pipe_  || !stdin_pipe_) {
        state_ = Error;
        return;
    }

    state_ = Running;
    if (ctx.stdin_fd == STDIN_FILENO)
        stdin_pipe_.close_write_end();

    pid_ = ::fork();
    if (pid_ == 0)
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
    
    if (pid_ < 0) state_ = Error;
}

Process::~Process() {
    ::waitpid(pid_, &status_, 0);
}

Pipe& Process::stdin_pipe() { return stdin_pipe_; }

Pipe& Process::stdout_pipe() { return stdout_pipe_; }

Pipe& Process::stderr_pipe() { return stderr_pipe_; }

bool Process::running() const { return state_ == Running; }

bool Process::want_stdin() { return stdin_pipe_.write_end() >= 0; }

bool Process::reaped() const {
    return state_ == Terminated;
}

pid_t Process::pid() const { return pid_; }

int   Process::status() const { return status_; }

void Process::kill() {
    ::kill(pid_, SIGKILL);
}

void Process::terminate() {
    ::kill(pid_, SIGTERM);
}

void Process::poll() {
    
    pid_t p = ::waitpid(pid_, &status_, WNOHANG);

    if (p == 0) return;

    if (p == pid_) state_ = Terminated;
}

ProcessResult Process::result() const {

    ProcessResult r;
    r.status = status_;

    if (WIFEXITED(status_))
        r.reason = Exited;
    else if (WIFSIGNALED(status_))
        r.reason = Signaled;
    else if (WIFSTOPPED(status_))
        r.reason = Stopped;
    else
        r.reason = Unknown;
    return r;
}

int Process::status_code(const ProcessResult& result) {

    switch (result.reason) {
        case ProcessExitReason::Signaled:
            return WTERMSIG(result.status) + 128;
        case ProcessExitReason::Exited:
            return WEXITSTATUS(result.status);
        case ProcessExitReason::Stopped:
            return W_STOPCODE(result.status);
        default: return 0; 
    }

    return 0;
}


}

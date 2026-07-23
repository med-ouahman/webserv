#include "Process.hpp"
#include <csignal>
#include <sys/wait.h>
#include <cstdlib>
#include <iostream>
#include "Result.hpp"
#include "CGIContext.hpp"

namespace cgi {

Process::Process()
    : state_(Setup),
    pid_(-1),
    status_(0),
    stdin_pipe_(),
    stdout_pipe_(),
    stderr_pipe_() {
    
    if (!stderr_pipe_ || !stdout_pipe_  || !stdin_pipe_) {
        state_ = Error;
        return;
    }

    state_ = Spawn;
}

Process::~Process() {}

Pipe& Process::stdin_pipe() { return stdin_pipe_; }

Pipe& Process::stdout_pipe() { return stdout_pipe_; }

Pipe& Process::stderr_pipe() { return stderr_pipe_; }

bool Process::running() const { return state_ == Running; }

bool Process::error() const { return state_ == Error; }

bool Process::want_stdin() { return stdin_pipe_.write_end() >= 0; }

bool Process::reaped() const {
    return state_ == Terminated;
}

pid_t Process::pid() const { return pid_; }

int   Process::status() const { return status_; }

void Process::kill() {
    if (state_ != Running) return;
    ::kill(pid_, SIGKILL);
}

void Process::terminate() {
    if (state_ != Running) return;
    ::kill(pid_, SIGTERM);
}

void Process::reap() {
    
    if (state_ == Terminated) return;
    
    pid_t p = ::waitpid(pid_, &status_, WNOHANG);

    if (p == pid_) {
        state_ = Terminated;
        pid_ = -1;
    }
    
}

ProcessResult Process::result() const {

    ProcessResult r;
    r.status = status_;

    if (WIFEXITED(status_)) r.reason = Exited;
    else if (WIFSIGNALED(status_)) r.reason = Signaled;
    else if (WIFSTOPPED(status_)) r.reason = Stopped;
    else r.reason = Unknown;

    return r;
}

int Process::status_code(const ProcessResult& result) {

    switch (result.reason) {
        case Signaled: return WTERMSIG(result.status) + 128;
        case Exited: return WEXITSTATUS(result.status);
        case Stopped: return W_STOPCODE(result.status);
        default: return 0; 
    }

    return 0;
}

bool Process::start(const ProcessContext& context) {

    if (state_ != Spawn) return true;

    if (context.stdin_fd.get() != STDIN_FILENO) stdin_pipe_.close_write_end();

    pid_ = ::fork();

    if (pid_ == 0) {
        if (context.stdin_fd.get() != STDIN_FILENO) {
            ::dup2(context.stdin_fd.get(), STDIN_FILENO);
            ::close(context.stdin_fd.release());
        } else {
            ::dup2(stdin_pipe_.read_end(), STDIN_FILENO);
        }

        ::dup2(stdout_pipe_.write_end(), STDOUT_FILENO);
        ::dup2(stderr_pipe_.write_end(), STDERR_FILENO);
        
        stdin_pipe_.close();
        stdout_pipe_.close();
        stderr_pipe_.close();

        ::execve(context.argv.argv()[0], context.argv.argv(), context.envp.argv());
        ::exit(EXIT_FAILURE);
    }

    stdin_pipe_.close_read_end();
    stdout_pipe_.close_write_end();
    stderr_pipe_.close_write_end();
    
    if (state_ == Error || pid_ < 0) {
        state_ = Error;
        return false;
    }

    state_ = Running;

    return true;
}

}

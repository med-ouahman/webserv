#include "Process.hpp"
#include <csignal>
#include <sys/wait.h>
#include <cstdlib>
#include "Result.hpp"
#include "CGIContext.hpp"
#include <cerrno>
#include <cstdio>
#include <fcntl.h>

namespace cgi {

static bool duplicateTo(int source, int destination) {
    if (source != destination)
        return ::dup2(source, destination) >= 0;

    int flags = ::fcntl(destination, F_GETFD);

    return flags >= 0 && ::fcntl(destination, F_SETFD, flags & ~FD_CLOEXEC) == 0;
}

Process::Process()
    : state_(Spawn),
    pid_(-1),
    status_(0),
    want_stdin_(true),
    stdin_pipe_(),
    stdout_pipe_(),
    stderr_pipe_() {

    if (!stderr_pipe_ || !stdout_pipe_  || !stdin_pipe_) {
        state_ = Error;
        return;
    }
}

Process::~Process() {
    if (pid_ <= 0)
        return;

    ::kill(pid_, SIGKILL);

    while (::waitpid(pid_, &status_, 0) < 0 && errno == EINTR) {}
    pid_ = -1;
    state_ = Terminated;
}

Pipe& Process::stdin_pipe() { return stdin_pipe_; }

Pipe& Process::stdout_pipe() { return stdout_pipe_; }

Pipe& Process::stderr_pipe() { return stderr_pipe_; }

bool Process::error() const { return state_ == Error; }

bool Process::want_stdin() { return want_stdin_; }

bool Process::reaped() const {
    return state_ == Terminated;
}

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

	else if (p < 0 && errno == ECHILD) {
		state_ = Terminated;
		pid_ = -1;
	}
    
}

ProcessResult Process::result() const {

    ProcessResult r;

    if (WIFEXITED(status_)) r.reason = Exited;
    else if (WIFSIGNALED(status_)) r.reason = Signaled;
    else r.reason = Unknown;

    return r;
}

bool Process::start(const ProcessContext& context) {

    if (state_ != Spawn) return true;

    if (context.stdin_fd.get() != STDIN_FILENO) {
        want_stdin_ = false;
        stdin_pipe_.close_write_end();
    } else {
        context.stdin_fd.release();
    }
    
    pid_ = ::fork();
    if (pid_ == 0) {

        if (want_stdin_) {
            if (!duplicateTo(stdin_pipe_.read_end(), STDIN_FILENO)) {
                LOG_ERROR(MAKE_ERRNO_ERROR("cgi::Process::dup2()"));
				::exit(1);
            }
        } else {
            if (!duplicateTo(context.stdin_fd.get(), STDIN_FILENO)) {
                LOG_ERROR(MAKE_ERRNO_ERROR("cgi::Process::dup2()"));
				::exit(1);
            }
            if (::close(context.stdin_fd.release()) == -1) {
                LOG_ERROR(MAKE_ERRNO_ERROR("cgi::Process::close()"));
				::exit(1);
            }
        }
        if (!duplicateTo(stdout_pipe_.write_end(), STDOUT_FILENO)) {
            LOG_ERROR(MAKE_ERRNO_ERROR("cgi::Process::dup2()"));
			::exit(1);
        }

        if (!duplicateTo(stderr_pipe_.write_end(), STDERR_FILENO)) {
            LOG_ERROR(MAKE_ERRNO_ERROR("cgi::Process::dup2()"));
			::exit(1);
        }
        stdin_pipe_.close();
        stdout_pipe_.close();
        stderr_pipe_.close();

        if (::chdir(context.working_dir.c_str()) == -1) {
            LOG_ERROR(MAKE_ERRNO_ERROR("cgi::Process::chdir()"));
			::exit(1);
        }

        ::execve(context.argv.argv()[0], context.argv.argv(), context.envp.argv());
        LOG_ERROR(MAKE_ERRNO_ERROR("cgi::Process::execve()"));
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

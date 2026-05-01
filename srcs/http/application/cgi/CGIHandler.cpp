#include "CGIHandler.hpp"
#include "EventLoop.hpp"
#include "Connection.hpp"
#include <signal.h>
#include <sys/wait.h>

namespace http {

    CGIHandler::CGIHandler( const core::Connection& con, const io::EventLoop& l )
        : cgi_state(SPAWN),
        cgi_pid(-1),
        cgi_status(0),
        pipe_guard(),
        stdin_ch(pipe_guard.stdin_pipe[1], this, STDStream::STDIN, EPOLLOUT | EPOLLET),
        stdout_ch(pipe_guard.stdout_pipe[0], this, STDStream::STDOUT, EPOLLIN | EPOLLET),
        stderr_ch(pipe_guard.stderr_pipe[0], this, STDStream::STDERR, EPOLLIN | EPOLLET),
        conn(con),
        loop(l) {}

    CGIHandler::~CGIHandler() {
        kill(cgi_pid, SIGKILL);
        waitpid(cgi_pid, &cgi_status, WNOHANG);
    }
}
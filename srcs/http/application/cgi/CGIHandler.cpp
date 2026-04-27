#include "CGIHandler.hpp"
#include "EventLoop.hpp"
#include "Connection.hpp"

namespace http {

    CGIHandler::CGIHandler( const io::EventLoop& l, const core::Connection& con )
        : cgi_state(SPAWN),
        loop(l),
        conn(con),
        cgi_pid(-1),
        stdin_ch(pipe_guard.stdin_pipe[1], this, Stream::STDIN, EPOLLOUT | EPOLLET),
        stdout_ch(pipe_guard.stdout_pipe[0], this, Stream::STDOUT, EPOLLIN | EPOLLET),
        stderr_ch(pipe_guard.stderr_pipe[0], this, Stream::STDERR, EPOLLIN | EPOLLET) {}

    CGIHandler::~CGIHandler() {}
}
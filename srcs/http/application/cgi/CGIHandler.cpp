#include "CGIHandler.hpp"
#include "EventLoop.hpp"
#include "Connection.hpp"
#include <signal.h>
#include <sys/wait.h>

namespace http {

    CGIHandler::CGIHandler( core::Connection& con, const io::EventLoop& l )
        : output_state(CGIOutputState::STATUS_LINE),
        cgi_state(CGIState::SPAWN),
        cgi_pid(-1),
        cgi_status(0),
        pipe_guard(),
        stdin_ch(pipe_guard.stdin_pipe[1], this, STDStream::STDIN, EPOLLOUT),
        stdout_ch(pipe_guard.stdout_pipe[0], this, STDStream::STDOUT, EPOLLIN),
        stderr_ch(pipe_guard.stderr_pipe[0], this, STDStream::STDERR, EPOLLIN),
        conn(con),
        loop(l),
        stdout_ch_view(stdout_ch.get_view()),
        scanner(stdout_ch.get_view()) {}

    CGIHandler::~CGIHandler() {
        ::kill(cgi_pid, SIGTERM);
        ::waitpid(cgi_pid, &cgi_status, WNOHANG);
        std::cout << "CGI CLEARED\n";
        std::cout << WEXITSTATUS(cgi_status) << "\n";
    }

    core::DataView& CGIHandler::get_stdout_data_view() {
        return stdout_ch_view;
    }
}
#include "CGIHandler.hpp"
#include "EventLoop.hpp"
#include "Connection.hpp"
#include <signal.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include "Timestamp.hpp"

namespace http {

    size_t CGIHandler::cgi_timeout_secs;

    const char* CGIHandler::cgi_metadata[] = {"REQUEST_METHOD", "SERVER_PROTOCOL", "QUERY_STRING", NULL};

    const char* CGIHandler::stripped_headers[] = {"transfer-encoding", "content-length", "content-type", "connection", NULL};

    CGIHandler::CGIHandler( core::Connection& conn_, const ResolutionResult res_ )
        : cgi_state(CGIState::SPAWN),
        cgi_pid(-1),
        cgi_status(0),
        pipe_guard(),
        stdin_ch(pipe_guard.stdin_pipe[1], *this, STDStream::STDIN, EPOLLOUT),
        stdout_ch(pipe_guard.stdout_pipe[0], *this, STDStream::STDOUT, EPOLLIN),
        stderr_ch(pipe_guard.stderr_pipe[0], *this, STDStream::STDERR, EPOLLIN),
        conn(conn_),
        result(res_),
        scanner(stdout_ch.get_view()),
        sigterm_sent_at(0) {}

    CGIHandler::~CGIHandler() {
        ::kill(cgi_pid, SIGKILL);
        ::waitpid(cgi_pid, &cgi_status, 0); // blocking
        std::cout << "CGI CLEARED\n";
        std::cout << WEXITSTATUS(cgi_status) << "\n";
    }

    void CGIHandler::handle() {
        
        try {
            spawn(conn.loop);
        } catch ( std::runtime_error& err ) {
            std::cout << err.what() << "\n";
            cgi_state = CGIState::ERROR;
            conn.on_cgi_error(http::INTERNAL_SERVER_ERROR);;
        }
    }

    bool CGIHandler::done() {
        return cgi_state == CGIState::FINISHED;
    }

    bool CGIHandler::timedout() {
        
        if (sigterm_sent_at.seconds() == 0) {

            if (start_time.elapsed() >= cgi_timeout_secs) {
                sigterm_sent_at.update();
                ::kill(cgi_pid, SIGTERM);
                if (0 == ::waitpid(cgi_pid, &cgi_status, WNOHANG))
                    return false;
                cgi_state = CGIState::ERROR;
                conn.on_cgi_error(GATEWAY_TIMEOUT);
                return true;
            }
            // Move to the destructor of CGIHandler
        } else if (sigterm_sent_at.elapsed() >= Limits::MAX_CGI_WAIT_AFTER_SIGTERM) {
            ::kill(cgi_pid, SIGKILL);

            ::waitpid(cgi_pid, &cgi_status, 0);  // should be blocking to ensuer the process is reaped
            conn.on_cgi_error(GATEWAY_TIMEOUT);
            cgi_state = CGIState::ERROR;
            return true;
        }

        return false;
    }


	void CGIHandler::on_ch_error() {
        std::cout << "IO CHANNEL ERROR\n";
        stdin_ch.shutdown();
        stdout_ch.shutdown();
        stderr_ch.shutdown();
		cgi_state = CGIState::ERROR;
	}

	bool CGIHandler::finished() {
		return cgi_state == CGIState::ERROR || cgi_state == CGIState::FINISHED;
	}

    DataView& CGIHandler::stdout_ch_data_view() {
        return stdout_ch.get_view();
    }

}
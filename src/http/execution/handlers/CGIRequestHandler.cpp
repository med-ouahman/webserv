#include <signal.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include "Timestamp.hpp"
#include "CGIRequestHandler.hpp"

namespace http {

    const char* CGIRequestHandler::cgi_metadata[] = {"REQUEST_METHOD", "SERVER_PROTOCOL", "QUERY_STRING", NULL};

    const char* CGIRequestHandler::stripped_headers[] = {"transfer-encoding", "content-length", "content-type", "connection", NULL};

    CGIRequestHandler::CGIRequestHandler(  const ResolutionResult res_ )
       ) {}

    CGIRequestHandler::~CGIRequestHandler() {
        ::kill(cgi_pid, SIGKILL);
        ::waitpid(cgi_pid, &cgi_status, 0); // blocking
        std::cout << "CGI CLEARED\n";
        std::cout << WEXITSTATUS(cgi_status) << "\n";
    }

    void CGIRequestHandler::handle() {
        
        try {
            spawn(conn.loop);
        } catch ( std::runtime_error& err ) {
            std::cout << err.what() << "\n";
            cgi_state = CGIState::ERROR;
            conn.on_cgi_error(http::INTERNAL_SERVER_ERROR);;
        }
    }

    bool CGIRequestHandler::done() {
        return cgi_state == CGIState::FINISHED;
    }

    bool CGIRequestHandler::timedout() {
        
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
            // Move to the destructor of CGIRequestHandler
        } else if (sigterm_sent_at.elapsed() >= Limits::MAX_CGI_WAIT_AFTER_SIGTERM) {
            ::kill(cgi_pid, SIGKILL);

            ::waitpid(cgi_pid, &cgi_status, 0);  // should be blocking to ensuer the process is reaped
            conn.on_cgi_error(GATEWAY_TIMEOUT);
            cgi_state = CGIState::ERROR;
            return true;
        }

        return false;
    }


	void CGIRequestHandler::on_ch_error() {
        std::cout << "IO CHANNEL ERROR\n";
        stdin_ch.shutdown();
        stdout_ch.shutdown();
        stderr_ch.shutdown();
		cgi_state = CGIState::ERROR;
	}

	bool CGIRequestHandler::finished() {
		return cgi_state == CGIState::ERROR || cgi_state == CGIState::FINISHED;
	}

    DataView& CGIRequestHandler::stdout_ch_data_view() {
        return stdout_ch.get_view();
    }

}
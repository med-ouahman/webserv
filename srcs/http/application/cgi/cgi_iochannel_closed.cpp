#include "CGIHandler.hpp"
#include "EventLoop.hpp"
#include "Connection.hpp"
#include <sys/wait.h>

namespace http {

	void CGIHandler::on_channel_closed( core::Connection& conn ) {

		if (cgi_state == CGIState::FINISHED) return ;

		cgi_state = CGIState::FINISHED;

		stdout_ch.shutdown();
		stderr_ch.shutdown();
		
		loop.del_fd(stdout_ch.get_fd());		
		loop.del_fd(stderr_ch.get_fd());
		
		conn.on_cgi_finished();
	}

}

#include "CGIHandler.hpp"
#include "EventLoop.hpp"
#include "Connection.hpp"
#include <sys/wait.h>

namespace http {

	void CGIHandler::on_channel_closed() {

		cgi_state = CGIState::FINISH;
		loop.del_fd(stdin_ch.get_fd());
		loop.del_fd(stdout_ch.get_fd());
		loop.del_fd(stderr_ch.get_fd());
		
		conn.on_cgi_finished();
	}

}

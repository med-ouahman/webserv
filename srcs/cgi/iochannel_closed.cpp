#include "CGIRequestHandler.hpp"
#include "EventLoop.hpp"
#include "Connection.hpp"
#include <sys/wait.h>

namespace http {

	void CGIRequestHandler::on_channel_closed() {

		if (cgi_state == CGIState::FINISHED) return ;

		std::cout << "IO CHANNEL CLOSED\n";
		cgi_state = CGIState::FINISHED;

		stdout_ch.shutdown();
		stderr_ch.shutdown();
		
		conn.loop.del_fd(stdout_ch.fd());		
		conn.loop.del_fd(stderr_ch.fd());
		
		conn.on_cgi_finished();
	}

}

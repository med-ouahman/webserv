#include "Connection.hpp"
#include "CGIBodyProvider.hpp"

namespace core {

	void Connection::on_cgi_finished() {
		state = ConnectionState::CGI_FINISH;
	}

	void Connection::on_cgi_output_ready() {
		response.body_provider = new http::CGIBodyProvider(*cgi_handler);
		state = ConnectionState::WRITING;
		action = ConnectionAction::WRITE;
	}

}

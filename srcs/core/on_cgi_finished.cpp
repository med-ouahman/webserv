#include "Connection.hpp"

namespace core {

	void Connection::on_cgi_finished() {
		state = ConnectionState::CGI_FINISH;
	}

}

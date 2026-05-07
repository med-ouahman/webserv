#include "CGIHandler.hpp"

namespace http {

	CGIState::Type CGIHandler::get_cgi_state() const {
		return cgi_state;
	}

	void CGIHandler::pull() {

		if (cgi_state == CGIState::ERROR || cgi_state == CGIState::FINISHED) {
			return ;
		}

		stdout_ch.process();
	}
	
	void CGIHandler::on_ch_error() {
		cgi_state = CGIState::WAITING;
	}

	bool CGIHandler::finished() {
		return cgi_state == CGIState::FINISHED;
	}
}

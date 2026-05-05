#include "CGIHandler.hpp"

namespace http {


	void CGIHandler::pull() {
		stdout_ch.process();
	}
	
}

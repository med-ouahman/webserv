#include "EventLoop.hpp"
#include "Connection.hpp"

namespace io {

	void EventLoop::pump() {

		for ( std::deque<core::Connection*>::iterator it = pending_conns.begin(); it != pending_conns.end(); ++it ) {
			(*it)->process();
		}

	}
}

#pragma once

#include <poll.h>
#include <stddef.h>

namespace runtime {
namespace poll {

class PollEventLoop {
private:
	const static size_t MAX_FDS = 1024;

	pollfd fds[MAX_FDS];
	size_t nfds;

	PollEventLoop(const PollEventLoop&);
	PollEventLoop& operator=(const PollEventLoop&);
public:
	void add_fd(const pollfd* fd);
	int run();
	PollEventLoop();
	~PollEventLoop();
};

}
}

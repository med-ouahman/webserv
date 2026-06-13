#pragma once

#include <stdint.h>
#include <sys/epoll.h>
#include "AEventHandler.hpp"

namespace runtime {
namespace epoll {

typedef uint32_t EpollEvent;

class EventPoller {
private:
	static const std::size_t	EpollMaxTimeoutMs = 1000;
	static const std::size_t	MaxMonitorFds = 1000;
	static const std::size_t	MaxEvents = 128;

	int epoll_fd;
	epoll_event events[MaxEvents];
	size_t monitor_count;
	
	EventPoller(const EventPoller& other);
	EventPoller& operator=(const EventPoller& other);
	
	static io::Event encode_events (EpollEvent ev);
	static EpollEvent decode_events(io::Event event);

public:
	EventPoller();
	~EventPoller();
	int poll();
	
	bool add(io::AEventHandler* handler);
	bool mod(io::AEventHandler* handler);
	bool del(io::AEventHandler* handler);
	void sync(io::AEventHandler*);
};

}}

#pragma once

#include <stdint.h>
#include <sys/epoll.h>
#include "AEventHandler.hpp"
#include "Logger.hpp"
#include "UniqueFd.hpp"

namespace runtime {
namespace epoll {

typedef uint32_t EpollEvent;

class EventLoop {
private:
	static const std::size_t	EpollMaxTimeoutMs = 1000;
	static const std::size_t	MaxMonitorFds = 1000;
	static const std::size_t	MaxEvents = 128;

	UniqueFd 		epoll_fd;
	bool			created_;
	epoll_event 	events[MaxEvents];
	size_t 			monitor_count;
	logger::Logger& logger;
	
	EventLoop(const EventLoop& other);
	EventLoop& operator=(const EventLoop& other);
	
	static io::Event encode_events (EpollEvent ev);
	static EpollEvent decode_events(io::Event event);

public:
	EventLoop();
	~EventLoop();
	int poll();
	
	bool add(io::AEventHandler* handler);
	bool mod(io::AEventHandler* handler);
	bool del(io::AEventHandler* handler);
	void sync(io::AEventHandler*);
	bool created() const;
};

}}

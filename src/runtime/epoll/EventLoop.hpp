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
	static const std::size_t	MaxMonitorFds = 4096;
	static const std::size_t	MaxEvents = 256;

	UniqueFd 		epoll_fd;
	bool			created_;
	epoll_event 	events[MaxEvents];
	size_t 			monitor_count;
	logger::Logger& logger;
	
	EventLoop(const EventLoop&);
	EventLoop& operator=(const EventLoop&);
	
	static io::Event encode_events (EpollEvent ev);
	static EpollEvent decode_events(io::Event event);

public:
	EventLoop(logger::Logger& log);
	~EventLoop();
	int poll();
	
	bool add(io::AEventHandler* handler);
	bool mod(io::AEventHandler* handler);
	bool del(io::AEventHandler* handler);
	void sync(io::AEventHandler* handler);
	
	bool created() const;
};

}}

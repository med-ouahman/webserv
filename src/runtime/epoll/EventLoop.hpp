#pragma once

#include "AEventHandler.hpp"
#include <stdint.h>
#include <iostream>
#include <sys/epoll.h>

namespace runtime {
namespace epoll {

typedef uint32_t EpollEvent;

typedef void (*ServerCallback)(void*);

class EventLoop {
private:
	static const std::size_t	EPOLL_TIMEOUT_MS = 1000;
	static const std::size_t	MAX_EVENTS = 128;

	int epoll_fd;
	epoll_event events[MAX_EVENTS];
		
	EventLoop(const EventLoop& other);
	EventLoop& operator=(const EventLoop& other);
	
	static io::Event encode_events (EpollEvent ev);
	static EpollEvent decode_events(io::Event event);

public:
	EventLoop();
	~EventLoop();
	
	int loop();
	
	bool register_handler(io::AEventHandler* handler);
	bool modify_handler(io::AEventHandler* handler);
	bool del_handler(io::AEventHandler* handler);
	
	void sync(io::AEventHandler*);
};

}}

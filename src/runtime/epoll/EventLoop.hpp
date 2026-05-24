#pragma once

#include <vector>
#include "AEventHandler.hpp"


namespace runtime {
namespace epoll {

typedef uint32_t EpollEvent;

class EventLoop {
private:
	int epoll_fd;
	static const std::size_t	EPOLL_TIMEOUT_MS = 1000;
	static const std::size_t	MAX_EVENTS = 128;
	EventLoop( const EventLoop& other );
	EventLoop& operator=( const EventLoop& other );

	static 	io::Event encode_events ( EpollEvent );
	static	EpollEvent decode_events(io::Event event);


public:
	EventLoop();
	~EventLoop();
	int run();
	bool register_handler( io::AEventHandler* handler );
	bool modify_handler( io::AEventHandler* handler );
	bool del_handler( io::AEventHandler* handler );
	void sync( io::AEventHandler* );
};

}}

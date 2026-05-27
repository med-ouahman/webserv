#pragma once

#include <vector>
#include "AEventHandler.hpp"
#include <stdint.h>
namespace runtime {

namespace epoll {

typedef uint32_t EpollEvent;

typedef void (*ServerCallback)( void* );

class EventLoop {
private:
	void* server_ctx;
	ServerCallback server_callback;
	int epoll_fd;
	static const std::size_t	EPOLL_TIMEOUT_MS = 1000;
	static const std::size_t	MAX_EVENTS = 128;
	EventLoop( const EventLoop& other );
	EventLoop& operator=( const EventLoop& other );
	static io::Event encode_events ( EpollEvent );
	static EpollEvent decode_events( io::Event event );

public:
	EventLoop( void* ctx, ServerCallback cb_ );
	~EventLoop();
	int run();
	bool register_handler( io::AEventHandler* handler );
	bool modify_handler( io::AEventHandler* handler );
	bool del_handler( io::AEventHandler* handler );
	void sync( io::AEventHandler* );
};

}}

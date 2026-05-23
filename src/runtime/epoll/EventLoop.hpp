#pragma once

#include <vector>
#include "AEventHandler.hpp"


namespace runtime {
namespace epoll {

class EventLoop {
private:
	int epoll_fd;

	EventLoop( const EventLoop& other );
	EventLoop& operator=( const EventLoop& other );

public:
	explicit EventLoop();
	~EventLoop();
	int run();

	bool register_handler( const io::AEventHandler* handler ) const;
	bool modify_handler( runtime::epoll::Event events, const io::AEventHandler* handler ) const;
	bool del_handler( const io::AEventHandler* handler ) const;
	
};

}

}

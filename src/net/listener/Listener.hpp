#pragma once

#include "AEventHandler.hpp"
#include "Result.hpp"
#include <vector>
#include "Config.hpp"

#ifndef NDEBUG
#define NDEBUG 4
#endif
#include <cassert>
#define BACKLOG 5

namespace net {

typedef void* AcceptContext;
typedef void (*AcceptCallback)( int client_fd, AcceptContext context );

class Listener: public io::AEventHandler {

private:
	Listener& operator=( const Listener& socket );
	bool accept_clients();
	bool on_error();
	AcceptCallback callback_;
	AcceptContext context_;

public:
	Listener( const Listener& socket );
	explicit Listener( int fd, io::Event mask, AcceptCallback cb, AcceptContext ctx );
	~Listener();
	void on_event( io::Event event );
	
};

Base::Result<Listener*>
create_listening_socket(
	const config::ListenEndPoint& endpoints,
	AcceptCallback cb,
	AcceptContext ctx );

}

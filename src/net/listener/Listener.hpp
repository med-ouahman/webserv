#pragma once

#include "AEventHandler.hpp"
#include "Result.hpp"
#include <vector>
#include "Config.hpp"

class Server;

#ifndef NDEBUG
#define NDEBUG 4
#endif
#include <cassert>
#define BACKLOG 5

namespace net {

class Listener: public io::AEventHandler {
private:

	enum ListenerState {
		Listening,
		ListenerError
	} state_;

	Server& webserv_;

	Listener(const Listener& socket);
	Listener& operator=(const Listener& socket);
	bool accept_clients();
	bool on_error();

public:
	explicit Listener(int fd, io::Event mask, Server& server);
	~Listener();
	void on_event(io::Event event);
	bool error() const;
};

base::Result<Listener*>
create_listening_socket(
	const config::ListenEndPoint& endpoints, Server& s);

}

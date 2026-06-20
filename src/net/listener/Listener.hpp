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

	std::string ip_;
	uint16_t	port_;

	Listener(const Listener& socket);
	Listener& operator=(const Listener& socket);
	bool accept_clients();
	bool on_error();

public:
	explicit Listener(int fd, io::Event mask, Server& server, std::string const& ip, uint16_t port);
	~Listener();
	void on_event(io::Event event);
	bool error() const;
};

base::Result<Listener*>
create_listening_socket(
	const config::ListenEndPoint& endpoints, Server& s);




std::string int_to_ip(uint32_t ip_addr);

}

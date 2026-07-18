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

typedef uint32_t IpAddress;
typedef uint16_t Port;

class Listener: public io::AEventHandler {
private:

	enum ListenerState {
		Listening,
		ListenerError
	} state_;

	Server& server_;

	uint32_t	host_;
	uint16_t	port_;

	const config::ListenEndPoint& endpoint_;
	std::vector<const config::ServerConfig*> servers_;
	
	Listener(const Listener& socket);
	Listener& operator=(const Listener& socket);

	bool accept_clients();
	bool on_error();

public:
	explicit Listener(int fd, io::Event mask, Server& server, const config::ListenEndPoint& ep);
	~Listener();
	void on_event(io::Event event);
	bool error() const;
	const config::ListenEndPoint& endpoint() const;
	void add_server(const config::ServerConfig* server);

	const std::vector<const config::ServerConfig*>& servers() const;
};

base::Result<Listener*>
create_listening_socket(
	const config::ListenEndPoint& endpoints, Server& s);

std::string int_to_ip(uint32_t ip_addr);

}

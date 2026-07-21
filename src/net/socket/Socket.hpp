#pragma once

#include "AEventHandler.hpp"
#include "Result.hpp"
#include <vector>
#include "Config.hpp"
#include "UniqueFd.hpp"

class Server;

#ifndef NDEBUG
#define NDEBUG 4
#endif
#include <cassert>
#define BACKLOG 5


namespace net {

typedef uint32_t IpAddress;
typedef uint16_t Port;

class Socket: public io::AEventHandler {
private:

	enum SocketState {
		Listening,
		SocketError
	} state_;

	Server& server_;

	uint32_t	host_;
	uint16_t	port_;

	const config::ListenEndPoint& endpoint_;
	std::vector<const config::ServerConfig*> servers_;
	
	Socket(const Socket& socket);
	Socket& operator=(const Socket& socket);

	bool accept_clients();
	bool on_error();

public:
	explicit Socket(UniqueFd& uniq, io::Event mask, Server& server, const config::ListenEndPoint& ep);

	~Socket();
	void on_event(io::Event event);
	bool error() const;
	const config::ListenEndPoint& endpoint() const;
	void add_server(const config::ServerConfig* server);

	const std::vector<const config::ServerConfig*>& servers() const;
};

base::Result<Socket*>
create_listening_socket(
	const config::ListenEndPoint& endpoints, Server& s);

std::string int_to_ip(uint32_t ip_addr);

}

#pragma once

#include "AEventHandler.hpp"
#include "Result.hpp"
#include <vector>
#include "Config.hpp"
#include "UniqueFd.hpp"
#include "logger/Logger.hpp"

class Server;

#ifndef NDEBUG
#define NDEBUG 4
#endif
#include <cassert>
#define BACKLOG 512


namespace net {



class Socket: public io::AEventHandler {
private:
	enum SocketState {
		Listening,
		SocketError
	} state_;

	Server& server_;
	logger::Logger& logger_;

	const config::ListenEndPoint& endpoint_;
	std::vector<const config::ServerConfig*> servers_;
	
	Socket(const Socket&);
	Socket& operator=(const Socket&);

	bool accept_clients();
	bool on_error();

	bool client_fd_conf(int fd);

public:
	explicit Socket(UniqueFd& uniq, io::Event mask, Server& server, const config::ListenEndPoint& ep);

	~Socket();
	void on_event(io::Event event);
	bool error() const;
	const config::ListenEndPoint& endpoint() const;
	void add_server(const config::ServerConfig* server);
};

base::Result<Socket*>
create_listening_socket(
	const config::ListenEndPoint& endpoints, Server& s);

std::string int_to_ip(uint32_t ip_addr);

bool listeners_match(const config::ListenEndPoint& existing, const config::ListenEndPoint& requested);

}

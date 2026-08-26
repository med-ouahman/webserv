#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "Result.hpp"
#include "Socket.hpp"
#include "Config.hpp"
#include <iostream>
#include "Server.hpp"
#include "UniqueFd.hpp"
#include <sstream>

namespace net {
	
Socket::Socket(UniqueFd& uniq, io::Event mask, Server& server, const config::ListenEndPoint& ep)
	: AEventHandler(uniq.release(), mask),
	state_(Listening),
	server_(server),
	logger_(server.logger()),
	endpoint_(ep) {}

Socket::~Socket() {}

void Socket::on_event(io::Event event) {
	
	switch (event) {
		case io::Readable:
			accept_clients();
			break;
		case io::Hup: case io::RHup:
			state_ = SocketError;
			break;
		case io::Error:
			on_error();
		default:
			break;
	}
}

bool Socket::accept_clients() {
	if (server_.num_connections() >= Server::MaxConnections) {
		logger_.log(logger::Warning, "Connection Limit reached, try again later", true);
		return false;
	}

	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	UniqueFd client(::accept(fd(), (struct sockaddr*)&client_addr, &client_addr_len));
	
	if (!client.valid()) return false;
	
	server_.add_connection(client, servers_);
	
	return true;
}

bool Socket::on_error() {
	state_ = SocketError;
	return false;
}

base::Result<Socket*> create_listening_socket(
	const config::ListenEndPoint& endpoint,
	Server& server
	) {

	logger::Logger& log = server.logger();

	sockaddr_in server_addr;
	
	::memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = endpoint.host;
	// if (!::inet_pton(AF_INET, int_to_ip(endpoint.host).c_str(), &server_addr.sin_addr)) return MAKE_ERRNO_ERROR("Socket::inet_pton()");
	server_addr.sin_port = ::htons(endpoint.port);

	UniqueFd socket_fd(::socket(AF_INET, SOCK_STREAM | O_NONBLOCK | SOCK_CLOEXEC, 0));
	if (!socket_fd.valid()) return MAKE_ERRNO_ERROR("Socket::socket()");

	int x = 1;
	if (::setsockopt(socket_fd.get(), SOL_SOCKET, SO_REUSEADDR, &x, sizeof x))
		return MAKE_ERRNO_ERROR("Socket::setsocketopt()");
		
	if (::bind(socket_fd.get(), (struct sockaddr *)&server_addr, sizeof server_addr)) return MAKE_ERRNO_ERROR("Socket::bind()");
	
	if (::listen(socket_fd.get(), BACKLOG) < 0) return MAKE_ERRNO_ERROR("Socket::listen()");

	net::Socket* sock = new (std::nothrow) Socket(socket_fd, io::Readable, server, endpoint);
	if (!sock) return MAKE_ERROR(Server::AllocFailed, "net::create_socket", "alloc failed");

	std::stringstream ss;
	ss << "Server Listening on " << int_to_ip(endpoint.host) << ":" << endpoint.port;
	log.log(logger::Info, ss.str(), true);

	return sock;
}

bool Socket::error() const {
	return state_ == SocketError;
}

void Socket::add_server(const config::ServerConfig* s) {
	servers_.push_back(s);
}


const config::ListenEndPoint& Socket::endpoint() const {
	return endpoint_;
}

std::string int_to_ip(uint32_t ip_addr)
{
    ip_addr = ntohl(ip_addr);

    std::ostringstream oss;

    oss << ((ip_addr >> 24) & 0xFF) << '.'
        << ((ip_addr >> 16) & 0xFF) << '.'
        << ((ip_addr >> 8) & 0xFF) << '.'
        << (ip_addr & 0xFF);

    return oss.str();
}


bool listeners_match(
    const config::ListenEndPoint& existing,
    const config::ListenEndPoint& requested
) {
    if (existing.port != requested.port) return false;

    if (existing.host == requested.host)  return false;

    if (existing.host == ::htonl(INADDR_ANY)) return true;

    return false;
}

}

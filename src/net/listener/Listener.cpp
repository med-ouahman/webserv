#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "Result.hpp"
#include "Listener.hpp"
#include "Config.hpp"
#include <iostream>
#include "Server.hpp"
#include <sstream>

namespace net {
	
Listener::Listener(int fd, io::Event mask, Server& server, const config::ListenEndPoint& ep)
	: AEventHandler(fd, mask),
	state_(Listening),
	server_(server),
	host_(ep.host),
	port_(ep.port),
	endpoint_(ep) {}

Listener::~Listener() {}

void Listener::on_event(io::Event event) {
	
	switch (event) {
		case io::Readable:
			std::cout << "Listener readable\n";
			accept_clients();
			break;
		case io::Hup: case io::RHup:
			/*
				let's see what to do here and defer this thing for later
			*/
			break;
		case io::Error:
			on_error();
		default:
			break;
	}
}

bool Listener::accept_clients() {

	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int client_fd = ::accept(fd(), (struct sockaddr*)&client_addr, &client_addr_len);
	
	if (client_fd < 0) return false;
	
	ConnectionInfo info(host_, port_, client_addr.sin_addr.s_addr, client_addr.sin_port);

	server_.add_connection(client_fd, info);
	
	return true;
}

bool Listener::on_error() {
	state_ = ListenerError;
	LOG_ERROR(MAKE_ERRNO_ERROR("EventPoller::accept()"));
	return false;
}


base::Result<Listener*> create_listening_socket(
	const config::ListenEndPoint& endpoint,
	Server& server
	) {

	sockaddr_in server_addr;
	::memset(&server_addr, 0, sizeof server_addr);
	server_addr.sin_family = AF_INET;
		
	if (!::inet_pton(AF_INET, int_to_ip(endpoint.host).c_str(), &server_addr.sin_addr))
		return MAKE_ERRNO_ERROR("EventPoller::create_listening_socket::inet_pton()");
	
	server_addr.sin_port = ::htons(endpoint.port);
	int socket_fd = ::socket(AF_INET, SOCK_STREAM | O_NONBLOCK | SOCK_CLOEXEC, 0);
	if (socket_fd < 0)
		return MAKE_ERRNO_ERROR("EventPoller::create_listening_socket::socket()");
	
	int x = 1;
	if (::setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &x, sizeof x))
		return MAKE_ERRNO_ERROR("EventPoller::create_listening_socket::setsocketopt()");
	
	if (::bind(socket_fd, (struct sockaddr *)&server_addr, sizeof server_addr))
		return MAKE_ERRNO_ERROR("EventPoller::create_listening_socket::bind()");
	
	if (::listen(socket_fd, BACKLOG) < 0)
		return MAKE_ERRNO_ERROR("EventPoller::create_listening_socket::listen()");

	std::stringstream ss;
	ss << "Server Listening on " << endpoint.host << ":" << endpoint.port;
	server.logger().log(logger::Info, ss.str(), true);

	return new Listener(socket_fd, io::Readable, server, endpoint);
}

bool Listener::error() const {
	return state_ == ListenerError;
}

void Listener::add_server(const config::ServerConfig* s) {
	servers_.push_back(s);
}


const config::ListenEndPoint& Listener::endpoint() const {
	return endpoint_;
}

std::string int_to_ip(uint32_t ip_addr) {
  	std::ostringstream oss;

    oss << ((ip_addr >> 24) & 0xFF) << '.'
        << ((ip_addr >> 16) & 0xFF) << '.'
        << ((ip_addr >> 8) & 0xFF) << '.'
        << (ip_addr & 0xFF);

    return oss.str();
}

}

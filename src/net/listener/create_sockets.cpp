
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include "Error.hpp"
#include "Result.hpp"
#include <vector>
#include "ListeningSocket.hpp"
#include "Config.hpp"

#include <iostream>

namespace net {

	Base::Result<ListeningSocket*> create_listening_socket(
		const config::ListenEndPoint& endpoint,
		AcceptCallback cb,
		AcceptContext ctx
		) {

		struct sockaddr_in server_addr;
		::memset(&server_addr, 0, sizeof server_addr);
		server_addr.sin_family = AF_INET;
			
		if (!::inet_pton(AF_INET, ::inet_ntoa((struct in_addr ){ .s_addr = endpoint.host }), &server_addr.sin_addr))
			return MAKE_ERRNO_ERROR("EventLoop::create_listening_socket::inet_pton()");
		
		server_addr.sin_port = ::htons(endpoint.port);
		int socket_fd = ::socket(AF_INET, SOCK_STREAM | O_NONBLOCK | SOCK_CLOEXEC, 0);
		if (socket_fd < 0)
			return MAKE_ERRNO_ERROR("EventLoop::create_listening_socket::socket()");
		
		int x = 1;
		if (::setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &x, sizeof x))
			return MAKE_ERRNO_ERROR("EventLoop::create_listening_socket::setsocketopt()");
		
		if (::bind(socket_fd, (struct sockaddr *)&server_addr, sizeof server_addr))
			return MAKE_ERRNO_ERROR("EventLoop::create_listening_socket::bind()");
		
		if (::listen(socket_fd, BACKLOG) < 0)
			return MAKE_ERRNO_ERROR("EventLoop::create_listening_socket::listen()");
		
		std::cout << "server listening on " <<  ::inet_ntoa((struct in_addr){ .s_addr = endpoint.host }) << ":"<< endpoint.port << '\n';
		return new ListeningSocket(socket_fd, EPOLLIN, cb, ctx);
	}
}

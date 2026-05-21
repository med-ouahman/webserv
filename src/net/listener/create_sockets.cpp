
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

	Base::Result<std::vector<ListeningSocket*> > create_listening_sockets(
		const std::vector<config::ListenEndPoint>& endpoints,
		AcceptCallback cb,
		AcceptContext* ctx
		) {

		std::vector<ListeningSocket*> listeners;

		struct sockaddr_in server_addr;
		::memset(&server_addr, 0, sizeof server_addr);
		server_addr.sin_family = AF_INET;
	
		for ( size_t i(0); i < endpoints.size(); ++i )
		{
			const config::ListenEndPoint& endpoint = endpoints[i];

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
			
			listeners.push_back(new ListeningSocket(socket_fd, EPOLLIN, cb, ctx));

			std::cout << "server listening on " <<  ::inet_ntoa((struct in_addr){ .s_addr = endpoint.host }) << ":"<< endpoint.port << '\n';
		}

		return listeners;
	}

	// bool EventLoop::start_listeners() {
	// 	config::ServerConfig server = config::Config::get_config().server;
 	// 	for ( ::size_t i(0); i < server.listens.size(); ++i ) {
	// 		error::Result<int> result = create_listening_socket(server.listens[i]);
			
	// 		if (!result.ok) {
	// 			LOG_ERROR(result.error);
	// 			return false;
	// 		}

	// 		ListeningSocket so(*this, result.result);
	// 		listeners.push_back(so);
	// 		so.release();
	// 	}

	// 	for ( ::size_t i(0); i < server.listens.size(); ++i ) {		
	// 		if (!register_handler(listeners[i].fd(), EPOLLIN, &listeners[i])) {
	// 			return false;
	// 		}
	// 	}
		
	// 	return true;
	// }
}

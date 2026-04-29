#include "EventLoop.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "Error.hpp"
#include "Result.hpp"

namespace io {

	error::Result<int> EventLoop::create_listening_socket( const config::ListenEndPoint& endpoint ) {
		struct sockaddr_in server_addr;
		::memset(&server_addr, 0, sizeof server_addr);
		server_addr.sin_family = AF_INET;
	
		if (!::inet_pton(AF_INET, ::inet_ntoa((struct in_addr ){ .s_addr = endpoint.host }), &server_addr.sin_addr)) {
			return MAKE_ERRNO_ERROR("EventLoop::create_listening_socket::inet_pton()");
		}

		server_addr.sin_port = ::htons(endpoint.port);
		int socket_fd = ::socket(AF_INET, SOCK_STREAM | O_NONBLOCK | SOCK_CLOEXEC, 0);
		if (socket_fd < 0) {
			return MAKE_ERRNO_ERROR("EventLoop::create_listening_socket::socket()");
		}
		
		int yes = 1;
		if (::setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes)) {
			return MAKE_ERRNO_ERROR("EventLoop::create_listening_socket::setsocketopt()");
		}
		
		if (::bind(socket_fd, (struct sockaddr *)&server_addr, sizeof server_addr)){
			return MAKE_ERRNO_ERROR("EventLoop::create_listening_socket::bind()");
		}
		
		if (::listen(socket_fd, BACKLOG) < 0) {
			return MAKE_ERRNO_ERROR("EventLoop::create_listening_socket::listen()");
		}

		std::cout << "server listening on " <<  ::inet_ntoa((struct in_addr){ .s_addr = endpoint.host }) << ":"<< endpoint.port << '\n';
		return error::Result<int>(socket_fd);
	}

	bool EventLoop::start_listeners( void ) {
 		for ( ::size_t i(0); i < conf.server.listens.size(); ++i ) {
			error::Result<int> result = create_listening_socket(conf.server.listens[i]);
			
			if (!result.ok) {
				LOG_ERROR(result.error);
				return false;
			}

			ListeningSocket so(*this, result.result);
			listeners.push_back(so);
			so.release();
		}

		/*
			DO not try to put both parts in the same loop.
			When we push back, addresses shift and epoll might point to an invalid address.
			Instead, we finish creating the socket and then add themt to epoll.
		*/

		for ( ::size_t i(0); i < conf.server.listens.size(); ++i ) {		
			if (!add_fd(listeners[i].get_fd(), EPOLLIN | EPOLLET, &listeners[i])) {
				return false;
			}
		}
		
		return true;
	}
}

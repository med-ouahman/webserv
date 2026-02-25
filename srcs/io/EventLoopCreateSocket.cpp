#include "EventLoop.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <cstring>

namespace io {

	int EventLoop::create_listening_socket( const config::ListenEndPoint& endpoint ) {
		struct sockaddr_in server_addr;
		memset(&server_addr, 0, sizeof server_addr);
		server_addr.sin_family = AF_INET;
		if (!inet_pton(AF_INET, inet_ntoa((struct in_addr){ .s_addr = endpoint.host }), &server_addr.sin_addr)) {
			perror("inet_pton");
			return -1;
		}
		server_addr.sin_port = htons(endpoint.port);
		int socket_fd = socket(AF_INET, SOCK_STREAM | O_NONBLOCK, 0);
		if (socket_fd < 0) {
			perror("socket");
			return -1;
		}
		int yes = 1;
		if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
			perror("setsockopt");
			return -1;
		}
		if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof server_addr)){
			perror("bind");
			return -1;
		}
		if (listen(socket_fd, BACKLOG) < 0) {
			perror("listen");
			return -1;
		}
		std::cout << "Server listening on IP: " <<  inet_ntoa((struct in_addr){ .s_addr = endpoint.host }) << " port: "<< endpoint.port << '\n';
		return socket_fd;
	}

	bool EventLoop::start_listeners( void ) {
 		for ( size_t i = 0; i < conf.server.listens.size(); i++ ) {
			int socket_fd = create_listening_socket(conf.server.listens[i]);
			listeners.push_back(ListeningSocket(*this, socket_fd));
		}

		for ( size_t i = 0; i < conf.server.listens.size(); ++i ) {		
			add_fd(listeners[i].get_fd(), EPOLLIN, &listeners[i]);
		}
		
		return true;
	}
}

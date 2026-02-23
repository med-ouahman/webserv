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
		inet_pton(AF_INET, inet_ntoa((struct in_addr){ .s_addr = endpoint.host }), &server_addr.sin_addr);
		server_addr.sin_port = htons(endpoint.port);
		int socket_fd = socket(AF_INET, SOCK_STREAM | O_NONBLOCK, 0);
		assert(socket_fd >= 0);
		int yes = 1;
		if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
			perror("setsockopt");
			_exit(1);
		}
		if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof server_addr)){
			perror("bind");
		}
		listen(socket_fd, BACKLOG);
		std::cout << "Server listening on localhost port: " << endpoint.port << '\n';
		return socket_fd;
	}
}

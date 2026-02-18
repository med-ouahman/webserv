#include "ListeningSocket.hpp"
#include "EventLoop.hpp"
#include "EventType.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <cstring>
namespace io {

	ListeningSocket::ListeningSocket( const ListeningSocket& socket ): loop(socket.loop) {
		(void)socket;
	}

	ListeningSocket& ListeningSocket::operator=( const ListeningSocket& socket ) {
		(void)socket;
		return *this;
	}

	ListeningSocket::ListeningSocket( EventLoop& loop ): server_fd(-1), loop(loop) {
		struct sockaddr_in server_addr;
		memset(&server_addr, 0, sizeof server_addr);
		server_addr.sin_family = AF_INET;
		inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
		server_addr.sin_port = htons(8080); // later will be Config::port
		server_fd = socket(AF_INET, SOCK_STREAM | O_NONBLOCK, 0);
		assert(server_fd >= 0);
		int yes = 1;
		if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
			perror("setsockopt");
			_exit(1);
		}
		if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof server_addr)){
			perror("bind");
		}
		listen(server_fd, BACKLOG);
		std::cout << "Server listening on localhost port: " << 8080 << '\n';
	}

	void ListeningSocket::on_event( io::EventType event ) {
		switch (event) {
			case READABLE:
				accept_clients();
				break;
			case ERROR:
				on_error();
			default:
				break;
		}
	};
}
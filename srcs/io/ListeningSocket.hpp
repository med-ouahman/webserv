#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#define NDEBUG 4
#include <cassert>
#define BACKLOG 5

namespace io {

	class ListeningSocket {
		private:
			int fd;
			ListeningSocket( const ListeningSocket& socket ) {
				(void)socket;
			}
			ListeningSocket& operator=( const ListeningSocket& socket ) {
				(void)socket;
				return *this;
			}
		public:
			int getFd() { return fd; }
			ListeningSocket() {
				struct sockaddr_in server_addr;
        		server_addr.sin_addr.s_addr = INADDR_ANY;
        		server_addr.sin_port = 8080;
        		int socketFd = socket(AF_INET, SOCK_STREAM, 0);
        		assert(socketFd >= 0);
        		int r = bind(socketFd, (struct sockaddr *)&server_addr, sizeof server_addr);
        		assert(r >= 0);
        		r = listen(socketFd, BACKLOG);
				assert(r >= 0);
			}
	};
}

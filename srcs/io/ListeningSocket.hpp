#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#ifndef NDEBUG
#define NDEBUG 4
#endif
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
			ListeningSocket(): fd(-1) {
				struct sockaddr_in server_addr;
        		server_addr.sin_addr.s_addr = INADDR_ANY;
        		server_addr.sin_port = 8080;
        		fd = socket(AF_INET, SOCK_STREAM | O_NONBLOCK, 0);
        		assert(fd >= 0);
        		int r = bind(fd, (struct sockaddr *)&server_addr, sizeof server_addr);
        		assert(r >= 0);
        		r = listen(fd, BACKLOG);
				assert(r >= 0);
				(void)r;
			}
	};
}

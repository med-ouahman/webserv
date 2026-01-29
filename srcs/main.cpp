#include <iostream>
#include <sys/epoll.h>
#include <fcntl.h>
#include <cstring>

static int usage( void ) {
	int ERR_USAGE = 1; /* WILL BE TRANSFORMED TO A MACRO */
	std::cerr << "Usage: ./webserv [ <config_file> ]\n";
	return ERR_USAGE;
}


int main( int argc, char** argv ) {

	if (argc > 2) {
		return usage();
	}
	int epollFd = epoll_create1(0);
	if (epollFd == -1) {
		std::cerr << "Error creating epoll file descriptor\n";
		return 1;
	}
	struct epoll_event s;
	memset(&s, 0, sizeof(s));
	return 0;
}


/*

man, this shit rocks 
I will finish this and then go to bed
yep

*/
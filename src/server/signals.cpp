#include <csignal>
#include <cstdlib>
#include <iostream>


namespace sys {

static void clear(int) {
	::system("clear");
}

static void close_gracefully(int) {

	std::cout << "Closing the server...\n";
	::exit(0);
}

void handle_signals() {
	signal(SIGPIPE, SIG_IGN);
	signal(SIGQUIT, clear);
	signal(SIGINT, close_gracefully);
}

}

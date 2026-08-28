#include <csignal>
#include <cstdlib>
#include <iostream>
#include "server/Server.hpp"

namespace sys {

static void close_server(int) {
	Server::shutdown();
}

void handle_signals() {
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, close_server);
	signal(SIGQUIT, close_server);
}

}

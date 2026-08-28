#include <csignal>
#include <cstdlib>
#include <iostream>


namespace sys {

void handle_signals() {
	signal(SIGPIPE, SIG_IGN);
}

}

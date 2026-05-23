#include <csignal>

namespace sys {

void handle_signals() {
	signal(SIGPIPE, SIG_IGN);
}

}

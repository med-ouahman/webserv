#include <csignal>

namespace sys {

<<<<<<< HEAD
    void handle_signals() {
        signal(SIGPIPE, SIG_IGN);
    }   
=======
void handle_signals() {
	signal(SIGPIPE, SIG_IGN);
}

>>>>>>> 2a4fb87 (s)
}

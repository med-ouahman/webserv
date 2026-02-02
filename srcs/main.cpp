
/*
  Load configuration from a file and initialize application settings.
*/
#include <sys/epoll.h>
#include <iostream>
#include <unistd.h>
int main( int argc, char* argv[] ) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }
    const char* configFile = argv[1];
    if (!configFile) {
        configFile = "config/default.conf";
    }
    int epollFd = epoll_create1(0);
    epoll_event event = {
        .events = EPOLLIN,
        .data = {.fd = STDIN_FILENO}
    };
    int _fd = STDIN_FILENO;
    int op = EPOLL_CTL_ADD;
    epoll_ctl(epollFd, op, _fd, &event);
    int events = epoll_wait(epollFd, &event, 10, -1);
    std::cout << "Events: " << events << '\n';
    return 0;
}


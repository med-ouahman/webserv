#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define MAX_EVENTS 10
#define MAX_INP_PIPES 4

int main( int argc, char* argv[] ) {
    
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(socket_fd != -1);
    int epfd = epoll_create1(0);

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = socket_fd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, socket_fd, &ev);
    assert(ret != -1);
    int r = epoll_wait(epfd, &ev, MAX_EVENTS, -1);
    assert(r != -1);
    close(epfd);
    close(socket_fd);
    return 0;
}

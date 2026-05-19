#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <sys/wait.h>

#define MAX_EVENTS 10
#define MAX_INP_PIPES 4

void read_fr( int f ){
    char b[101];
    printf("> ");
    b[read(f, b, 100)] = 0;
    printf("%s\n", b);
}

void write_fr( int fd ) {
    char b[100];
    read(0, b, 100);
    write(fd, b, 100);
}

int main( int argc, char* argv[] ) {
    
    int epfd = epoll_create1(EPOLL_CLOEXEC);

    struct epoll_event ev1, ev2;
    int fds[4];
    pipe(fds);

    int a = fork();
    if (fork() == 0) {
        dup2(fds[1] , 1);
        close(fds[0]);
        close(fds[1]);

        execve("./p", argv, __environ);
        _exit(1);
    }

    ev1.events = EPOLLOUT;
    ev1.data.fd = fds[0];

    ev2.events = EPOLLIN;
    ev2.data.fd = fds[1];
    epoll_ctl(epfd, EPOLL_CTL_ADD, fds[0], &ev1);
    epoll_ctl(epfd, EPOLL_CTL_ADD, fds[1], &ev2);

    struct epoll_event events[MAX_EVENTS];
    while (true) {
        int n = epoll_wait(epfd, events, MAX_EVENTS, -1);

        printf("n: %d\n", n);
        for ( int i = 0; i < n; i++ ) {
            if (events[i].events&EPOLLIN) {
                read_fr(events[i].data.fd);
            } else {
                write_fr(events[i].data.fd);
            }
        }
    }

    waitpid(a, 0, 0);

    close(epfd);
    return 0;
}

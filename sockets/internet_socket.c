// internet sockets

// these one file programs are just for learning purposes. In real applications, you would typically separate the code into multiple files and use header files for declarations.

// usual includes for socket programming
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <asm-generic/socket.h>
#include <signal.h>
#include <fcntl.h>
#define BACKLOG 5

int signum = 0;
#define assert(expr) \
    do { \
        if (!(expr)) { \
            fprintf(stderr, "Assertion failed: %s, file %s, line %d\n", #expr, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)

void handle_sigint(int sig) {
    signum = sig;
}

void handle_request(int client_fd) {
    
    char buffer[1024];
    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    assert(bytes_received >= 0 && "Failed to receive data from client");
    buffer[bytes_received] = '\0'; // null-terminate the buffer
    // check if the url has /shutdown
    if (strstr(buffer, "GET /shutdown") != NULL) {
        printf("Shutdown request received. Stopping server...\n");
        signum = SIGINT; // set the signal to trigger shutdown
        return;
    }
    printf("Received request:\n%s\n", buffer);
}

void handle_connection(int client_fd) {
    int htmlfd = open("index.html", O_RDONLY);
    assert(htmlfd >= 0 && "Failed to open index.html");
    char buffer[1024];

    // accept response
    handle_request(client_fd);
    ssize_t bytes_read;
    while ((bytes_read = read(htmlfd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0'; // null-terminate the buffer
        ssize_t bytes_sent = send(client_fd, buffer, bytes_read, 0);
        assert(bytes_sent >= 0 && "Failed to send data to client");
    }
    close(htmlfd);
}

int main( void ) {

    struct sockaddr_in sock_addr = {
        .sin_family = AF_INET, // IPv4
        .sin_port = htons(8080), // port number (convert to network byte order)
        .sin_addr.s_addr = inet_addr("127.0.0.1")
    };

    int sock_fd = socket(sock_addr.sin_family, SOCK_STREAM | SOCK_NONBLOCK, 0);
    assert(sock_fd >= 0 && "Failed to create socket");
    int r = bind(sock_fd, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
    assert(r == 0 && "Failed to bind socket");
    r = listen(sock_fd, BACKLOG);
    assert(r == 0 && "Failed to listen on socket");
    int client_fd = -1;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    printf("Server is listening on port 8080...\n");
    signal(SIGINT, handle_sigint);
    while (1) {
        if (signum == SIGINT) {
           
            break;
        }
        client_fd = accept(sock_fd, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_fd < 0)
            continue;
        assert(client_fd >= 0 && "Failed to accept connection");
        printf("Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        // close(client_fd);
        handle_connection(client_fd);
    }
    printf("Shutting down server...\n");
    close(sock_fd);
    return 0;
}

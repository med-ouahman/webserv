/*
    this is the server program that listens for incoming connections on a specified IP address and port.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <asm-generic/socket.h>
#include <sys/un.h>
#define NDEBUG
#include <assert.h>
/*
    Understandig socket:
    - A socket is an endpoint for communication between two machines.
    - It is an abstraction that allows applications to send and receive data over a network.
    - Sockets can be used for various types of communication, including TCP, UDP, and
        raw sockets.
    - Sockets are created using the socket() system call, which returns a file descriptor

    the socket() function takes three arguments:
    int socket(int domain, int type, int protocol);
    - domain: Specifies the communication domain (e.g., AF_INET for IPv4, AF_INET6 for IPv6).
    - type: Specifies the communication type (e.g., SOCK_STREAM for TCP, SOCK_DGRAM for UDP).
    - protocol: Specifies the protocol to be used (e.g., IPPROTO_TCP for TCP, IPPROTO_UDP for UDP). If set to
        0, the system will choose the appropriate protocol based on the domain and type.
*/

int parse_port(const char* port_str) {
    char* endptr;
    long port = strtol(port_str, &endptr, 10);
    if (*endptr != '\0' || port < 1 || port > 65535) {
        fprintf(stderr, "Invalid port number: %s\n", port_str);
        exit(EXIT_FAILURE);
    }
    return (int)port;
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <socket_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    /* UNIX-domain socket example */
    struct sockaddr_un sock_addr;
    // set everything to zero
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sun_family = AF_UNIX;
    // this will create a socket file named "socket_file" in the current directory. The
    // server should be set up to listen on this socket file for incoming connections.
    strncpy(sock_addr.sun_path, argv[1], sizeof(sock_addr.sun_path) - 1);
     int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        assert(0 && "Failed to create socket");
    }
    if (bind(sockfd, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) < 0) {
        assert(0 && strerror(errno));
    }
    if (listen(sockfd, 5) < 0) {
        assert(0 && strerror(errno));
    }
    printf("Server is listening on socket file: %s\n", sock_addr.sun_path);
    char buffer[1024];
    while (1) {
        int clientfd = accept(sockfd, NULL, NULL);
        if (clientfd < 0) {
            
            continue; // continue accepting other connections
        }
        printf("clientfd: %d\n", clientfd);
        printf("Accepted a connection from a client\n");
        ssize_t num_read = read(clientfd, buffer, sizeof(buffer) - 1);
        if (num_read < 0) {
            assert(0 && "Failed to read from client");
        }
        if (num_read == 0) {
            // client closed the connection
            close(clientfd);
            return 0;
        }
        buffer[num_read] = '\0'; // null-terminate the string
        printf("Received message from client: %s\n", buffer);
        close(clientfd); // close the client socket after handling the request
    }
    return 0;
}

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

int main( int argc, char* argv[] ) {

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IP_ADDRESS e.g 127.0.0.1> <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int ip = inet_addr(argv[1]);
    int port = parse_port(argv[2]);
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = ip;
    server_addr.sin_port = htons(port);
    bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(sockfd, 1);
    // the loop to accept incoming connections
    char buffer[1024];
    while (1) {
        int client_sockfd = accept(sockfd, NULL, NULL);
        if (client_sockfd < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        // handle the client connection (e.g., read/write data)
        recv(client_sockfd, buffer, sizeof(buffer), 0);
        printf("Received data: %s\n", buffer);
        // check if client closed the connection
        close(client_sockfd);
    }
    return 0;
}

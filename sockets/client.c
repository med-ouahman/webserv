
/*
    this is the client program that connects to a server at a specified IP address and port, sends a message, and receives a response. It uses the socket API to create a socket, connect to the server, send data, and receive data. The client takes the server's IP address and port number as command-line arguments.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <asm-generic/socket.h>
#include <readline/readline.h>
#define NDEBUG
#include <assert.h>

int main( int argc, char* argv[] ) {

    
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IP_ADDRESS> <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // create client socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    // set up server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid IP address: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    // connect to server
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }
    // send message to server
    const char* message = readline("Enter message to send: ");
    if (send(sockfd, message, strlen(message), 0) < 0) {
        perror("send");
        exit(EXIT_FAILURE);
    }
}
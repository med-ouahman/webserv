
/*
    this is the client program that connects to a server at a specified IP address and port, sends a message, and receives a response. It uses the socket API to create a socket, connect to the server, send data, and receive data. The client takes the server's IP address and port number as command-line arguments.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <asm-generic/socket.h>
#include <readline/readline.h>
#define NDEBUG
#include <assert.h>
#include <errno.h>
#include <sys/stat.h>

int main( int argc, char* argv[] ) {

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <socket_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // check if the socket file exists
    struct stat st;
    if (stat(argv[1], &st) < 0) {
        fprintf(stderr, "Socket file %s does not exist\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    /* UNIX-domain socket example */
    struct sockaddr_un sock_addr;
    // set everything to zero 
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sun_family = AF_UNIX;
    // this will create a socket file named "socket_file" in the current directory. The server should be set up to listen on this socket file for incoming connections.
    strncpy(sock_addr.sun_path, argv[1], sizeof(sock_addr.sun_path) - 1);

    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        assert(0 && "Failed to create socket");
    }
    // bind the socket to the address
    if (connect(sockfd, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) < 0) {
        
        assert(0 && strerror(errno));
    }
    // read from stdin
    char buffer[1024];
    int num_read = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
    if (num_read < 0) {
        assert(0 && "Failed to read from stdin");
    }
    buffer[num_read] = '\0'; // null-terminate the string
    // send the message to the server using write (will use send later)
    if (write(sockfd, buffer, num_read) != num_read) {
        assert(0 && "failed/partial write to socket");
    }
    close(sockfd); // server sees EOF when client closes the socket
    return 0;
}
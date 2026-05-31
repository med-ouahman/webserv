#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <cstring>
#include <iostream>

#define IP 1
#define PORT 11
int null_index = -1;

uint32_t extract(char* d, int type) {
    char *to_parse = NULL;
    if (type == IP) {
        int len = ::strlen(d);
        d[len] = 0;
        to_parse = d;
    } else if (type == PORT) {
        if (null_index != -1) {
            to_parse = d + null_index+1;
            return atoi(to_parse);
        } else {
            return 0;
        }
    }
    int data = atoi(to_parse);
    if (data < 0) {
        return 0;
    }
    return uint32_t(data);
}

int main(int argc, char** argv) {
    
    if (argc != 2) {
        std::cerr << "Usage:\n" << argv[0] << " IP:PORT\n";
        return 1;
    }
    uint32_t ip = extract(argv[0], IP);
    uint16_t port = extract(argv[0], PORT);
}
#include "EventLoop.hpp"
#include "ListeningSocket.hpp"
#include <iostream>
#include "ConfigParser.hpp"
#include <signal.h>
#include <stdlib.h>

void clear( int a ) {
    if ( a== SIGQUIT){

        system("clear");
    }
}

int main( int argc, char** argv ) {
    if (argc > 2) {
        std::cerr << "Usage:\n./webserv [ configuration file ]";
        return 1;
    }
    signal(SIGQUIT, clear);
    const char* config_file = argv[1];
    if (!config_file) {
        config_file = "./config/default.conf";
    }
    #ifdef DEV_MODE
    config::Config conf = config::ConfigParser::build_default_config();
    #endif
    io::EventLoop event_poop(conf);
    event_poop.run();
    return 0x0;
}

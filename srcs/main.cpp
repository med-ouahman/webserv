#include "EventLoop.hpp"
#include "ListeningSocket.hpp"
#include <iostream>
#include "ConfigParser.hpp"
#include <signal.h>
#include <stdlib.h>
#include "CGIBodyProvider.hpp"

int main( int argc, char** argv ) {

    if (argc > 2) {
        std::cerr << "Usage:\n./webserv [ configuration file ]\n";
        return 1;
    }

    signal(SIGPIPE, SIG_IGN);
    
    const char* config_file = argv[1];
    if (!config_file) {
        config_file = "./config/default.conf";
    }
    
    #ifdef DEV_MODE
    config::Config conf = config::ConfigParser::build_default_config();
    #endif
    
    io::EventLoop loop(conf);
    return loop.run();
}

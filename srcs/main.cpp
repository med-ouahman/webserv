#include "EventLoop.hpp"
#include "ListeningSocket.hpp"
#include <iostream>
#include "ConfigParser.hpp"
#include <signal.h>
#include <stdlib.h>
#include "CGIBodyProvider.hpp"

bool config::Config::has_instance = false;

config::Config config::Config::instance;

int main( int argc, const char* argv[] ) {

    if (argc > 2) {
        std::cerr << "Usage:\n./webserv [ configuration file ]\n";
        return 1;
    }

    signal(SIGPIPE, SIG_IGN);
    
    const char* config_file = argv[1];
    if (not config_file) {
        config_file = "./config/default.conf";
    }
    
    #ifdef DEV_MODE
    config::Config conf = config::ConfigParser::build_default_config();
    config::Config::set_config(conf);
    #endif
    
    io::EventLoop loop;
    return loop.run();
}

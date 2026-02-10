#include "EventLoop.hpp"
#include <iostream>

int main( int argc, char* argv[] ) {
    if (argc > 3) {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }

    const char* configFile = argv[1];
    if (!configFile) {
        configFile = "config/default.conf";
    }
    io::EventLoop eventLoop;
    eventLoop.run();
    return 0;
}


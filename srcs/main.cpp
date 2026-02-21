#include "EventLoop.hpp"
#include "ListeningSocket.hpp"
#include <iostream>
#include "ConfigParser.hpp"

int main( int argc, char* argv[] ) {
    
    if (argc > 3) {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }

    const char* configFile = argv[1];
    if (!configFile) {
        configFile = "config/default.conf";
    }

    config::Config conf = config::ConfigParser::build_default_config();
    io::EventLoop eventLoop(conf);
    io::ListeningSocket server(eventLoop);
    eventLoop.run(server);
    return 0;
}

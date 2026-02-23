#include "EventLoop.hpp"
#include "ListeningSocket.hpp"
#include <iostream>
#include "ConfigParser.hpp"

int main( int argc, char* argv[] ) <%

    if (argc > 3) {
        std::cerr << "Usage: " << argv[0] << " [ config_file ]" << std::endl;
        return 1;
    }
    const char* config_file = argv[1];
    if (!config_file) {
        config_file = "config/default.conf";
    }
    config::Config conf = config::ConfigParser::build_default_config();
    io::EventLoop event_poop(conf);
    event_poop.run();
    return 0;
%>

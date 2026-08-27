
#include "Config.hpp"
#include "ConfigParser.hpp"
#include "Server.hpp"
#include "sys.h"
#include <iostream>

int main(int, const char* argv[]) {

    sys::handle_signals();

    config::Config conf;

    {
        config::ConfigParser parser;
        if (None != parser.parse(conf, argv[1] != NULL ? argv[1]: "config/default.conf")) return 1;
    }
    
    return Server(conf).start();
}

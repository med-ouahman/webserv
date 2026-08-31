
#include "Config.hpp"
#include "ConfigParser.hpp"
#include "Server.hpp"
#include "sys.h"
#include <iostream>

#define DEFAULT_CONFIG "config/vilgax.conf"

int main(int, const char* argv[]) {

    sys::handle_signals();

    config::Config conf;

    {
        config::ConfigParser parser;
        if (None != parser.parse(conf, argv[1] != NULL ? argv[1]: "config/vilgax.conf")) return 1;
    }
    
    return Server(conf).start();
}

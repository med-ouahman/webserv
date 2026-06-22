
#include "Config.hpp"
#include "ConfigParser.hpp"
#include "Server.hpp"
#include "sys.h"

namespace config {
    bool Config::has_instance = false;
    Config Config::instance;
}


int main(int argc, const char* argv[]) {

    if (argc > 2) {
        std::cerr << "Usage:\n./webserv [ configuration file ]\n";
        return 1;
    }

    sys::handle_signals();

    std::string configfile = argv[1] != NULL ? argv[1]: "config/webserv.conf";
    
    config::Config conf = config::ConfigParser::build_default_config();
    
    config::Config::set(conf);

    Server webserv;
    
    return webserv.start();
}

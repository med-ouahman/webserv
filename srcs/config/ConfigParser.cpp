
#include "ConfigParser.hpp"

namespace config {
    ConfigParser::ConfigParser() {

    }
    
    ConfigParser::~ConfigParser() {

    }

    Config parse( const char* config_path ) {
        
    }

    #ifdef DEV_MODE
    Config ConfigParser::build_default_config() {
        Config conf;

        return conf;
    }
    #endif
}
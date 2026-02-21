#pragma once

#include "Config.hpp"
#include <vector>
#include <string>

#define DEV_MODE 0

namespace config {

    class ConfigParser {
        private:
            std::vector<std::string> tokens;
            void tokenize( void );
        public:
            Config parse( const char* config_path );
            ConfigParser();
            ~ConfigParser();
            #ifdef DEV_MODE
            static Config build_default_config();
            #endif
    };
}
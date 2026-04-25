#pragma once

#include <string>
#include <stdint.h>

namespace http {
    
    struct CGIContext {
        std::string script_filename;      // "/var/www/cgi-bin/script.php"
        std::string interpreter_path;     // "/usr/bin/php-cgi"
        std::string path_info;            // "/user/123"
        std::string script_name;          // "/cgi-bin/script.php"
        std::string working_directory;    // "/var/www/cgi-bin"
        std::string server_name;          // "localhost"
        uint32_t timeout_seconds;     // 30
        uint16_t server_port;       // 8080   
    };
}

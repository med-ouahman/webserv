#pragma once

#include <string>
#include <stdint.h>

namespace http {
    
    struct CGIContext {
        std::string temp_body_path;
        std::string script_filename;      
        std::string interpreter_path;     
        std::string path_info;
        std::string script_name;          
        std::string working_directory;
        std::string server_name;          
        uint32_t timeout_seconds;     
        uint16_t server_port;       
    };
}

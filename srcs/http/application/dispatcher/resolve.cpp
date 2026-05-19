#include "Dispatcher.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include <sys/stat.h>

namespace http {
    
    std::string Dispatcher::extract_path( const std::string& url ) {
        return url;
    }

    bool Dispatcher::file_exists( const char* filename ) {
        struct stat buf;

        if (stat(filename, &buf)) {
            return false;
        }
        
        return true;
    }

    ResolutionResult Dispatcher::resolve( const Request& req ) {
        ResolutionResult result(req);

        config::ServerConfig server = config::Config::get_config().server;
        
        result.path = "";
        result.status_code = http::OK;
        result.type = ResponseType::CGI;
        result.location = &config::Config::get_config().server.locations[0];
        return result;
        result.location = find_location(req.data().unparsed_uri, server.locations);
        const std::string path = extract_path(req.data().unparsed_uri);
        bool method_allowed = true;
        
        if (not result.location) {
            result.status_code = INTERNAL_SERVER_ERROR;
            result.type = ResponseType::ERROR_RESPONSE;
            result.path = "";
            return result;
        }
        
        if (result.location->redirect.return_code) {
        }

        if (result.location->allowed_methods.find(req.get_method_name()) == result.location->allowed_methods.end()) {
            method_allowed = false;
        }

        if (req.data().method == DELETE) {
            if (!method_allowed) {
            }

            if (file_exists(path.c_str())) {
                result.type = ResponseType::FILE_DELETE;
                result.path = path;
                return result;
            }
        }


        return result;
    }
}


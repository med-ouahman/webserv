#include "HTTPDispatcher.hpp"
#include "Config.hpp"
#include "HTTPRequest.hpp"
#include <sys/stat.h>

namespace http {
    
    std::string HTTPDispatcher::extract_path( const std::string& url ) {
        return url;
    }

    bool HTTPDispatcher::file_exists( const char* filename ) {
        struct stat buf;

        if (stat(filename, &buf)) {
            return false;
        }
        
        return true;
    }

    ResolutionResult HTTPDispatcher::resolve( const HTTPRequest& req ) {
        ResolutionResult result(req);

        config::ServerConfig server = config::Config::get_config().server;
        
        result.path = "";
        result.reason = "";
        result.status_code = http::OK;
        result.type = HTTPResponseType::CGI;
        return result;
        const config::LocationConfig* location = find_location(req.url, server.locations);
        const std::string path = extract_path(req.url);
        bool method_allowed = true;
        
        if (!location) {
            result.status_code = INTERNAL_SERVER_ERROR;
            result.type = HTTPResponseType::ERROR_RESPONSE;
            result.path = "";
            return result;
        }
        
        if (location->redirect.return_code) {
            return ResolutionResult(static_cast<HTTPStatusCode>(location->redirect.return_code), "Redirect");
        }

        if (location->allowed_methods.find(req.get_method(req.method)) == location->allowed_methods.end()) {
            method_allowed = false;
        }

        if (req.method == DELETE) {
            if (!method_allowed) {
                return ResolutionResult(METHOD_NOT_ALLOWED, "Method Not Allowed");
            }

            if (file_exists(path.c_str())) {
                result.type = HTTPResponseType::FILE_DELETE;
                result.path = path;
                return result;
            }
            return ResolutionResult(NOT_FOUND, "Not Found");
        }


        return result;
    }
}


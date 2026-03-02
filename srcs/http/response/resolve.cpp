#include "HTTPResponseHandler.hpp"
#include "Config.hpp"
#include "HTTPRequest.hpp"

namespace http {
    
    HTTPResponseHandler::ResolutionResult HTTPResponseHandler::resolve( const HTTPRequest& req, const config::ServerConfig& server ) {
        ResolutionResult result;

        const config::LocationConfig* location = find_location(req.url, server.locations);
        
        if (!location) {
            result.status_code = NOT_FOUND;
            result.type = ERROR_RESPONSE;
            result.path = "";
        }
        
        return result;
    }
}


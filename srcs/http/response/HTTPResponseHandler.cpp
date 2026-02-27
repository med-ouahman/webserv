
#include "HTTPRequest.hpp"
#include "HTTPResponseHandler.hpp"
#include "Config.hpp"


namespace http {

    HTTPResponseHandler::HTTPResponseHandler() {}

    HTTPResponseHandler::~HTTPResponseHandler() {}

    std::string HTTPResponseHandler::serialize( void ) const {
        return response.body;
    }

    void HTTPResponseHandler::handle_request( const HTTPRequest& req ) {
        (void)req;
    }

    HTTPResponseHandler::ResolutionResult HTTPResponseHandler::resolve( const HTTPRequest& req, const config::ServerConfig& server ) const {
        ResolutionResult result;

        const config::LocationConfig* location = find_location(req.url, server.locations);
        
        if (!location) {
            result.status_code = NOT_FOUND;
            result.type = ERROR_RESPONSE;
            result.path = "";
        }
        
        return result;
    }

    const config::LocationConfig* HTTPResponseHandler::find_location( const std::string& url, const std::vector<config::LocationConfig>& locations ) {
    
        size_t location_index = locations.size();
        size_t best_prefix_length = 0;

        for ( size_t i = 0; i < locations.size(); ++i ) {
            if (url == locations[i].path) {
                return &locations[i];
            }
            if (!url.compare(0, locations[i].path.size(), locations[i].path) && url[locations[i].path.size()] == '/') {
                if (best_prefix_length < locations[i].path.size()) {
                    best_prefix_length = locations[i].path.size();
                    location_index = i;
                }
            }
        }

        if (location_index == locations.size()) {
            return NULL;
        }

        return &locations[location_index];
    }

    void HTTPResponseHandler::build_error_response( HTTPStatusCode code, std::string reason ) {
        response.status_code = code;
        response.reason = reason;
        response.headers["Content-Length"] = "0";
        response.headers["Connection"] = "close";
    }

}

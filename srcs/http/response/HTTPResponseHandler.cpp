
#include "HTTPRequest.hpp"
#include "HTTPResponseHandler.hpp"
#include "Config.hpp"

// password = "MOMDAD1234@@me";
namespace http {

    HTTPResponseHandler::HTTPResponseHandler() {
        
    }

    HTTPResponseHandler::~HTTPResponseHandler() {

    }

    std::string HTTPResponseHandler::serialize( void ) const {
        return response.body;
    }

    void HTTPResponseHandler::handle_request( const HTTPRequest& req ) {
        (void)req;
    }

    HTTPResponseHandler::ResolutionResult HTTPResponseHandler::resolve( const HTTPRequest& req, const config::ServerConfig& server ) const {
        ResolutionResult result;

        const config::LocationConfig* location = find_location(req.url, server);

    }

    const config::LocationConfig* HTTPResponseHandler::find_location( const std::string& url, const config::ServerConfig& server ) {
       
    }

}

/*
    /images/favicon.svg
    
    /images
    /im
    /assets



*/

#include "HTTPRequest.hpp"
#include "HTTPResponseHandler.hpp"

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
        
    }
}

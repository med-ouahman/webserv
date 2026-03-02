#include "HTTPResponseHandler.hpp"
#include "HTTPRequest.hpp"

namespace http {
    bool HTTPResponseHandler::handle_request( const HTTPRequest& req ) {
        
        if (!req.version_supported()) {
            build_error_response(http::VERSION_NOT_SUPPORTED, "Version not supported");
            serialize();
            return true;
        }

        return false;
    }
}

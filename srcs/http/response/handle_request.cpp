#include "HTTPResponseHandler.hpp"
#include "HTTPRequest.hpp"
#include <fcntl.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include "Config.hpp"

namespace http {


    void HTTPResponseHandler::handle_request( const HTTPRequest& req ) {
        // ResolutionResult result = resolve(req, config.server);
        // send dummy response.
        (void)req;
        response.body = "HTTP/1.1 200 OK\r\n\r\n";
    }
}

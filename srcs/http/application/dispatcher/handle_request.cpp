#include "HTTPDispatcher.hpp"
#include "HTTPRequest.hpp"
#include <fcntl.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include "Config.hpp"

namespace http {

    void HTTPDispatcher::handle_request( ResolutionResult& result, const HTTPRequest& req ) {
        
        (void)req;
        switch (result.type) {
            case HTTPResponseType::ERROR_RESPONSE:
                build_error_response(result.status_code, result.reason);
                break;
            case HTTPResponseType::STATIC_FILE:
                /* static file handler */
                break;
            case HTTPResponseType::DIRECTORY:
                generate_directory_list(result.path.c_str());
                break;
            case HTTPResponseType::REDIRECT:
                /* redirect handler */
                break;
            case HTTPResponseType::FILE_UPLOAD:
                /* upload file handler */
                break;
            case HTTPResponseType::CGI:
                /* deferred to the cgi handler */
            default:
                break;
        };
    }
}

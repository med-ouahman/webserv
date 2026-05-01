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


    HTTPDispatcher::HandlerResult HTTPDispatcher::handle_request( const HTTPRequest& req ) {
        
        ResolutionResult result = resolve(req, config.server);
        HandlerResult handler_result;

        handler_result.response_type = result.type;
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
                handler_result.cgi_ctx = get_cgi_context(req, result);
                break;
            default:
                break;
        }
        
        return handler_result;
    }
}

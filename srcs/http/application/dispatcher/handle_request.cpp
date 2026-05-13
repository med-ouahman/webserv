#include "HTTPDispatcher.hpp"
#include "HTTPRequest.hpp"
#include <fcntl.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include "Config.hpp"
#include "IRequestHandler.hpp"
#include "CGIHandler.hpp"
#include "Connection.hpp"

namespace http {

    IRequestHandler* HTTPDispatcher::dispatch_request_handler( core::Connection& conn, const HTTPRequest& req ) {
        
        (void)req;
        ResolutionResult& result=conn.current_res;

        switch (result.type) {
            case HTTPResponseType::ERROR_RESPONSE:
                build_error_response(result.status_code, result.reason);
                // reutnr ErrorHandler()
                break;
            case HTTPResponseType::STATIC_FILE:
                /* static file handler */
                // return new StaticFileHandler()
                break;
            case HTTPResponseType::DIRECTORY:
                generate_directory_list(result.path.c_str());
                break;
                // return new DirectoryHandler()
            case HTTPResponseType::REDIRECT:
                /* redirect handler */
                // return new RedirectHandler();
                break;
            case HTTPResponseType::FILE_UPLOAD:
                /* upload file handler */
                // return new FileUploadHandler()
                break;
            case HTTPResponseType::CGI:
                return new CGIHandler(conn);
            default:
                break;
        };

        return NULL;
    }
}

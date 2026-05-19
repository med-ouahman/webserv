#include "Dispatcher.hpp"
#include "Request.hpp"
#include <fcntl.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include "Config.hpp"
#include "IRequestHandler.hpp"
#include "CGIRequestHandler.hpp"
#include "Connection.hpp"

namespace http {

    IRequestHandler* Dispatcher::create_request_handler( core::Connection& conn, const ResolutionResult& result ) {
        
        switch (result.type) {
            case ResponseType::ERROR_RESPONSE:
                // build_error_response(result.status_code);
                // reutnr ErrorHandler()
                break;
            case ResponseType::STATIC_FILE:
                /* static file handler */
                // return new StaticFileHandler()
                break;
            case ResponseType::DIRECTORY:
                generate_directory_list(result.path.c_str());
                break;
                // return new DirectoryHandler()
            case ResponseType::REDIRECT:
                /* redirect handler */
                // return new RedirectHandler();
                break;
            case ResponseType::FILE_UPLOAD:
                /* upload file handler */
                // return new FileUploadHandler()
                break;
            case ResponseType::CGI:
                return new CGIRequestHandler(conn, result);
            default:
                break;
        };

        return NULL;
    }
}

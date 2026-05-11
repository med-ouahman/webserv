
#include "HTTPRequest.hpp"
#include "HTTPDispatcher.hpp"
#include "Config.hpp"
#include <iostream>
#include <cstdlib>

namespace http {
    
    
    HTTPDispatcher::HTTPDispatcher() {}

    HTTPDispatcher::~HTTPDispatcher() {}

    void HTTPDispatcher::build_error_response( HTTPStatusCode code, std::string reason ) {
        std::cout << "Sure shit, code: " << code << " reason: " << reason << "\n";
        HTTPResponse response;
        response.status_code = code;
        response.reason = reason;
        response.headers["Connection"] = "close";
        response.body = "HTTP/1.1 403 Forbidden\r\nConnection: close\r\n\r\n";
    }


    CGIContext HTTPDispatcher::get_cgi_context( const HTTPRequest& req, const ResolutionResult& result ) {
        
        CGIContext ctx;

        (void)req;
        (void)result;
        
        std::string home(getenv("HOME"));
        ctx.script_filename   = home + (home[home.length() - 1] == '/' ? "":"/") + "/cgi-bin/hello.py";
        
        ctx.interpreter_path  = "/usr/bin/python3";
        ctx.script_name       = "/cgi-bin/cgi_py.py";
        ctx.path_info         = "/user/42";
        ctx.working_directory = "/home/mouahman/cgi-bin";
        ctx.server_name       = "localhost";
        ctx.server_port       = 8080;
        ctx.timeout_seconds   = 5;

        return ctx;
    }

    void HTTPDispatcher::configure_body_policy( const HTTPRequest& req, ResolutionResult& result ) {
        
        result.body_policy.type = detect_body_type(const_cast<std::map<std::string, std::string>&>(req.headers));
        
        if (result.body_policy.type == BodyType::NONE) {
            return ;
        }

        result.body_policy.body_path = result.path;

        char* end = NULL;        
        result.body_policy.parsed_body_size = ::strtoul(const_cast<HTTPRequest&>(req).headers["content-length"].c_str(), &end, 10);
        
        if (end && *end != '\0') {
            result.status_code = BAD_REQUEST;
            result.reason = "Bad Request";
            result.body_policy.type = BodyType::ERROR;
            return ;
        }

        if (result.body_policy.parsed_body_size > config::Config::get_config().server.client_max_body_size) {
            result.status_code = PAYLOAD_TOO_LARGE;
            result.reason = "Payload Too Large";
            result.body_policy.type = BodyType::ERROR;
            return ;
        }

        if (result.type == HTTPResponseType::CGI) {
            result.body_policy.storage = BodyStorage::FILE_TEMP;
            return ;
        }

        result.body_policy.storage = BodyStorage::FILE_PERM;
        
        if (result.body_policy.parsed_body_size < MAX_BUFFERED_BODY_SIZE) {
            result.body_policy.storage = BodyStorage::BUFFER;
        }
    }

}

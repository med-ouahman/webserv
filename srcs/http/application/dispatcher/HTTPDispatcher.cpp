
#include "HTTPRequest.hpp"
#include "HTTPDispatcher.hpp"
#include "Config.hpp"
#include <iostream>
#include <cstdlib>

namespace http {
    
    
    HTTPDispatcher::HTTPDispatcher( const config::Config& conf )
        :config(conf),
        allow_keep_alive(true)
         {}

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

}

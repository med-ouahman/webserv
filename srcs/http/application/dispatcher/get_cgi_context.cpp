#include "HTTPDispatcher.hpp"
#include <cstdlib>
namespace http {


    CGIContext HTTPDispatcher::get_cgi_context( const HTTPRequest& req, const ResolutionResult& result ) {
        
        CGIContext ctx;

        (void)req;
        (void)result;
        
        std::string home(getenv("HOME"));
        ctx.script_filename   = home + (home[home.length() - 1] == '/' ? "":"/") +"webserv/cgi-bin/script.py";
        ctx.interpreter_path  = "/usr/bin/python3";
        ctx.script_name       = "/cgi-bin/hello.py";
        ctx.path_info         = "/user/42";
        ctx.working_directory = "/home/mouahman/cgi-bin";
        ctx.server_name       = "localhost";
        ctx.server_port       = 8080;
        ctx.timeout_seconds   = 5;

        return ctx;
    }

}

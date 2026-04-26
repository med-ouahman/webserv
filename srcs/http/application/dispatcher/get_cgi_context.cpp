#include "HTTPDispatcher.hpp"

namespace http {


    CGIContext HTTPDispatcher::get_cgi_context( const HTTPRequest& req, const ResolutionResult& result ) {
        CGIContext  context;
        
       CGIContext ctx;

        ctx.script_filename   = "/home/mouahman/cgi-bin/hello.py";
        ctx.interpreter_path  = "/usr/bin/python3";
        ctx.script_name       = "/cgi-bin/hello.py";
        ctx.path_info         = "/user/42";
        ctx.working_directory = "/home/mouahman/cgi-bin";
        ctx.server_name       = "localhost";
        ctx.server_port       = 8080;
        ctx.timeout_seconds   = 5;

        return context;
    }

}

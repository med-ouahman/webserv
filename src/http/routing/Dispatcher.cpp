
#include "Request.hpp"
#include "Dispatcher.hpp"
#include "Config.hpp"
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>

namespace http {
    
    
    Dispatcher::Dispatcher() {}

    Dispatcher::~Dispatcher() {}

    CGIRequestContext Dispatcher::resolve_cgi_context( const ResolutionResult& result ) {
        
        CGIRequestContext ctx;

        (void)result;
        char* d = getcwd(NULL, 0);
        std::string cgi_bin(d);
        free(d);
    
        ctx.script_filename   = cgi_bin + "/cgi-bin/hello.py";
        ctx.interpreter_path  = "/usr/bin/python3";
        ctx.script_name       = "/cgi-bin/cgi_py.py";
        ctx.path_info         = "/user/42";
        ctx.working_directory = "/home/mouahman/cgi-bin";
        ctx.server_name       = "localhost";
        ctx.server_port       = 8080; // LATER
        ctx.timeout_seconds   = result.location->cgi_timeout;

        return ctx;
    }

    BodyConf Dispatcher::configure_body( const Request& req, const ResolutionResult& result ) {
        BodyConf body;
        
        body.type = detect_body_type(req);
        
        if (body.type == BodyType::NONE) {
            return body;
        }

        char* end = NULL;        
        body.parsed_body_size = ::strtoul(req.data().headers.get("content-length").c_str(), &end, 10);
        
        if (end && *end not_eq '\0') {
            std::cout << "Bad Content length\n";
            body.type = BodyType::ERROR;
            return body;
        }

        if (body.parsed_body_size > config::Config::get_config().server.client_max_body_size) {
            body.type = BodyType::ERROR;
            return body;
        }

        if (result.type == ResponseType::CGI) {
            body.storage = BodyStorage::FILE_TEMP;
            return body;
        }

        body.path = result.path;
        body.storage = BodyStorage::FILE_PERM;
        
        if (body.parsed_body_size < MAX_BUFFERED_BODY_SIZE) {
            body.storage = BodyStorage::BUFFER;
        }

        return body;
    }


    size_t Dispatcher::file_size( const std::string& filename ) {

        struct stat buf;
        stat(filename.c_str(), &buf);

        return buf.st_size;
    }
}

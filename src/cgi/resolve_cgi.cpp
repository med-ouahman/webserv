
#include "EnvBuilder.hpp"
#include <iomanip>
#include <sstream>
#include "Request.hpp"
#include "Dispatcher.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

namespace cgi {

std::string EnvBuilder::transform(bool has_http_prefix, http::Headers::const_iterator& it) {

    std::string result;

    const std::string http_prefix = has_http_prefix ? "HTTP_": "";
    size_t size = it->name.size() + it->value.size() + http_prefix.size() + 1;
    
    result.reserve(size);
    result.append(http_prefix);
    
    std::string name;

    for (size_t i =  0; i < it->name.size(); ++i) {
        char c = it->name[i];
        if (c == '-') name += '_';
        else name += std::toupper(c);
    }
    
    result.append(name);
    result.append("=");
    result.append(it->value);
    
    return result;
}

http::Headers EnvBuilder::build_metadata(const CGIRequestContext& ctx) {

    http::Headers headers;

    headers.add("REQUEST_METHOD", ctx.request_method);
    headers.add("SCRIPT_NAME", ctx.script_name);
    headers.add("PATH_INFO", ctx.path_info);
    headers.add("SERVER_PROTOCOL", ctx.server_protocol);
    headers.add("GATEWAY_INTERFACE", "CGI/1.1");
    headers.add("CONTENT_TYPE", ctx.mime_type);
    headers.add("CONTENT_LENGTH", ctx.content_length);
    headers.add("SERVER_PORT", ctx.server_port);

    return headers;
}

CStringArray EnvBuilder::build(const CGIRequestContext& ctx,
    http::Headers const& request_headers) {

    http::Headers headers = build_metadata(ctx);
   
    http::Headers::iterator it = headers.begin();
    
    for (; it != headers.end(); ++it) std::cout << it->name << " " << it->value << "\n";

    CStringArray arr;
    
    size_t env_size(0);for(;__environ[env_size];++env_size);
    
    arr.push_array(const_cast<const char**>(__environ), env_size);

    for (http::Headers::const_iterator it = headers.begin();
            it != headers.end();
            ++it) {
        arr.push(transform(false, it).c_str());
    }

    for ( http::Headers::const_iterator it = request_headers.begin();
        it != request_headers.end(); ++it ) {
           
            arr.push(transform(true, it));
    }

    return arr;
}

CGIExecContext resolve(http::Request const& req, const http::ResolutionResult& r) {
    
    CGIRequestContext req_ctx;
    (void)req;
    (void)r;

    req_ctx.query_string = "query";
    req_ctx.interpreter = "./cgi_tester";
    req_ctx.content_length = "100";
    req_ctx.mime_type = "text/html";
    req_ctx.request_method = "GET";
    req_ctx.server_protocol = "HTTP/1.1";
    req_ctx.server_port = "3000";
    req_ctx.server_name = "webserv";
    req_ctx.script_name = "";
    req_ctx.path_info = "path info come on";

    CGIExecContext exec_ctx;
    http::Headers req_headers;

    exec_ctx.stdin_fd = STDIN_FILENO;
    
    exec_ctx.envp = EnvBuilder::build(req_ctx, req_headers);
    exec_ctx.argv.push(req_ctx.interpreter);
    exec_ctx.argv.push("cgi-bin/hello.py");
    
    return exec_ctx;
}

}

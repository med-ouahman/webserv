
#include "EnvBuilder.hpp"
#include <iomanip>
#include <sstream>
#include "Request.hpp"
#include "Dispatcher.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

namespace cgi {

const char* EnvBuilder::metadata[] = {"REQUEST_METHOD", "SERVER_PROTOCOL", "QUERY_STRING", "GATEWAY_INTERFACE", NULL};
const char* EnvBuilder::stripped_headers[] = {"transfer-encoding", "content-length", "content-type", "connection", NULL};

bool EnvBuilder::forbidden_header(const std::string& header_name) {

    for (size_t i(0); stripped_headers[i] != NULL; ++i) {
        if (stripped_headers[i] == header_name) return true;
    }

    return false;
}

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

    for (size_t i(0); metadata[i] != NULL; ++i) {
        std::string s = std::string(metadata[i]);
        if (s == "REQUEST_METHOD") {
            headers.add(s, ctx.request_method);
        } else if (s == "SERVER_PROTOCOL") {
            headers.add(s, "HTTP/1.1");
        } else if (s == "QUERY_STRING") {
            headers.add(s, ctx.query_string);
        } else if (s == "CONTENT_TYPE") {
            headers.add(s, ctx.mime_type);
        } else if (s == "CONTENT_LENGTH") {
            std::stringstream ss;
            ss << std::dec << ctx.body_content_length;
            headers.add(s, ss.str());
        } else if (s == "GATEWAY_INTERFACE") {
            headers.add(s, "CGI/1.1");
        } else if (s == "SCRIPT_NAME") {
            headers.add(s, ctx.script_name);
        } else if (s == "PATH_INFO") {
            headers.add(s, ctx.path_info);
        } else if (s == "SERVER_NAME") {
            headers.add(s, ctx.server_name);
        } else if (s == "SERVER_PORT") {
            std::stringstream ss;
            ss << std::dec << ctx.server_port;
            headers.add(s, ss.str());
        }
    }

    return headers;
}

CStringArray EnvBuilder::build(const CGIRequestContext& ctx,
    http::Headers const& request_headers) {

    http::Headers headers = build_metadata(ctx);
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
            if (forbidden_header(it->name)) continue;
            arr.push(transform(true, it));
    }

    return arr;
}

CGIExecContext resolve_exec_context(http::Request const& req, const http::ResolutionResult& r) {
    
    CGIRequestContext req_ctx;
    (void)req;
    (void)r;
    req_ctx.body_content_length = 100;
    req_ctx.body_filename = "/tmp/some";
    req_ctx.query_string = "qery";
    req_ctx.interpreter = "/bin/python3";
    CGIExecContext exec_ctx;
    http::Headers req_headers;

    exec_ctx.stdin_fd = STDIN_FILENO;
    if (!req_ctx.body_filename.empty())
        exec_ctx.stdin_fd = ::open(req_ctx.body_filename.c_str(), O_RDONLY);
    exec_ctx.envp = EnvBuilder::build(req_ctx, req_headers);
    exec_ctx.argv.push(req_ctx.interpreter);
    exec_ctx.argv.push("cgi-bin/hello.py");
    return exec_ctx;
}

}

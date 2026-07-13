#pragma once

#include "CStringArray.hpp"
#include "http/common/Headers.hpp"
#include <stdint.h>

namespace http { class Request; struct ResolutionResult; }

namespace cgi {

struct CGIRequestContext {

    std::string request_method;
    std::string mime_type;
    std::string interpreter;
    std::string script_name;
    std::string query_string;
    std::string content_length;
    std::string path_info;

    std::string server_name;
    std::string server_protocol;
    std::string server_port;
};

struct CGIExecContext {
    std::string working_dir;
    int         stdin_fd;

    CStringArray argv;
    CStringArray envp;
};

class EnvBuilder {
private:
    static std::string transform(bool has_http_prefix, http::Headers::const_iterator& it);
    static http::Headers build_metadata(const CGIRequestContext& context);
    
public:
    static CStringArray build(const CGIRequestContext& ctx,
       http::Headers const& request_headers);
};

CGIExecContext resolve(const http::Request& req,
    const http::ResolutionResult& res);

}
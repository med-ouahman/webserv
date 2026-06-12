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
    std::string body_filename;
    size_t		timeout_seconds;
    size_t      body_content_length;
    std::string path_info;
    std::string server_name;
    uint16_t    server_port;
};

struct CGIExecContext {
    std::string working_dir;
    int         stdin_fd;
    size_t      timeout_seconds;
    CStringArray argv;
    CStringArray envp;
};


class EnvBuilder {
private:
    const static char* metadata[];
    const static char* stripped_headers[];
    
    static std::string transform(bool has_http_prefix, http::Headers::const_iterator& it);
    static http::Headers build_metadata(const CGIRequestContext& context);
	
    static bool forbidden_header(const std::string& header_name);    
public:
    static CStringArray build(const CGIRequestContext& ctx,
       http::Headers const& request_headers);
};

CGIExecContext resolve_exec_context(const http::Request& req, const http::ResolutionResult& res);

}
#pragma once

#include "CStringArray.hpp"
#include "UniqueFd.hpp"
#include "Request.hpp"
#include "http/common/Headers.hpp"

namespace http { struct ResolutionResult; }

namespace cgi {

class CGIResolver {
public:
    struct CGIRequestContext {
        std::string request_method;
        std::string mime_type;
        std::string interpreter;
        std::string script_name;
        std::string query_string;
        std::string working_dir;
        std::string body_filename;

        size_t		timeout_seconds;
        size_t      body_content_length;

        std::string path_info;
        std::string server_name;
        uint16_t    server_port;
    };

    struct CGIExecContext {
        std::string interpreter;
        std::string working_dir;
        
        UniqueFd  stdin_fd;
        size_t    timeout_seconds;

        CStringArray argv;
        CStringArray envp;

        // CGIExecContext() {}
    };

    struct Context {
        CGIRequestContext req_;
        CGIExecContext exec_;
    };

    static Context resolve(http::Request const& req, const http::ResolutionResult& r);

private:
    static CGIRequestContext request_context( http::Request const& req, const http::ResolutionResult& r );
    static CGIExecContext	 execution_context( const CGIRequestContext& ctx,
        http::Headers const& request_headers );
};

}
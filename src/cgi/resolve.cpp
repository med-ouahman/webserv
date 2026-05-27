#include "cgi.hpp"
#include "Request.hpp"
#include "Dispatcher.hpp"
#include <fcntl.h>

namespace cgi {

namespace resolver {

Context resolve( http::Request const& req, const http::ResolutionResult& r ) {
    Context ctx;
    CGIRequestContext& req_ctx = ctx.req_;
    (void)req;
    req_ctx.body_content_length = 100;
    req_ctx.body_filename = "/tmp/some";
    req_ctx.query_string = r.query_string;

    CGIExecContext& exec_ctx = ctx.exec_;

    exec_ctx.stdin_fd.set(::open(req_ctx.body_filename.c_str(), O_RDONLY));
    exec_ctx.argv = envp::Builder::build(ctx.req_, req.headers_);
    return ctx;
}

}
}

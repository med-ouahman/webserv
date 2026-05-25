#include "CGIResolver.hpp"
#include "Request.hpp"
#include "CGIEnvBuilder.hpp"
#include "Dispatcher.hpp"

namespace cgi {

http::Headers transform_headers( std::map<std::string, std::string> const& h ) {
http::Headers hh;

    for ( std::map<std::string, std::string>::const_iterator it = h.begin(); it != h.end(); ++it) {
        hh.add(it->first, it->second);
    }

    return hh;
}

CGIResolver::Context CGIResolver::resolve( http::Request const& req, const http::ResolutionResult& r ) {
    Context ctx;
    ctx.req_ = request_context(req, r);
    http::Headers hh = transform_headers(req.headers);
    ctx.exec_ = execution_context(ctx.req_, hh);
    return ctx;
}

CGIResolver::CGIRequestContext CGIResolver::request_context( const http::Request& request, const http::ResolutionResult& result ) {
    CGIRequestContext ctx;

    ctx.query_string = result.query_string;
    ctx.body_content_length = request.body.size();
    ctx.body_filename = "/tmp/cgi";
    return ctx;
}

CGIResolver::CGIExecContext CGIResolver::execution_context( const CGIResolver::CGIRequestContext& request_ctx
    , http::Headers const& request_headers ) {

    CGIExecContext ctx;

    ctx.argv = CGIEnvBuilder::build(request_ctx, request_headers);

    return ctx;
}

}

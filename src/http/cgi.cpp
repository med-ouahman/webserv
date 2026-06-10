#include "Context.hpp"
#include "CGIRequestHandler.hpp"

namespace http {
 
void Context::on_cgi_ready(const CGIResult& result) {

    CGIRequestHandler* h = static_cast<CGIRequestHandler*>(handler);

}

}
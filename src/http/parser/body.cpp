
#include "parse.hpp"
#include "Context.hpp"
namespace http {
    namespace parser {
        Error	parse_body(Context& ctx) {
            if (0 == ctx.body_received) return http::EBAD_REQUEST;
            return NONE;
        }
    }
}


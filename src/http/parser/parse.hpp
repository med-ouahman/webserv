
#pragma once

#include <string>

#include "http/Error.hpp"

namespace http {

class Context;

namespace parser {

bool	get_chunk(Context& ctx, std::string& out);

Error	parse(Context& ctx);
Error	parse_request_line(Context& ctx);
Error	parse_headers(Context& ctx);
Error	parse_body(Context& ctx);

}
}

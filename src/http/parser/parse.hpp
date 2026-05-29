
#pragma once

#include <string>

#include "http/Error.hpp"

namespace http {

class Context;

namespace parser {

Error	get_chunk(Context& ctx, std::string& out, bool& found);

Error	parse(Context& ctx);
Error	parse_request_line(Context& ctx);
Error	parse_headers(Context& ctx);
Error	parse_body(Context& ctx);

}
}

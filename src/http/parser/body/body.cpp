#include "http/parser/body/body.hpp"
#include "http/Context.hpp"

namespace http {

Error Context::parse_body() {
	if (request.chunked)
		return parse_chunked_body();
	return parse_fixed_body();
}

}

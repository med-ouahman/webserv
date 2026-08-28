
#include "http/Parser/Parser.hpp"
#include "http/Parser/headers/headers.hpp"
#include "http/Context.hpp"
#include "base/base.hpp"

namespace http {

struct Entity {
	usize colon;
	std::string name;
	std::string value;
	std::string normalized;
};


static http::Error	prepareHeaderLine(const std::string& line, Entity& entity) {

	entity.colon = line.find(':');
	if (entity.colon == std::string::npos || entity.colon == 0)
		return http::ERR_BAD_REQUEST;

	entity.name = line.substr(0, entity.colon);
	entity.value = line.substr(entity.colon + 1);
	parser::trim(entity.name);
	parser::trim(entity.value);
	if (entity.name.empty() || entity.value.empty())
		return http::ERR_BAD_REQUEST;

	entity.normalized = base::toLowerCase(entity.name);
	return http::ERR_NONE;
}

namespace parser {

static Error parseHeaderLine(http::Request& request, const std::string& line) {

	Entity entity;
	http::Error err;

	TRY(prepareHeaderLine(line, entity), err);
	TRY(storeHeader(request, entity.name, entity.value), err);
	TRY(handleSpecialHeader(request, entity.normalized, entity.value), err);
	return ERR_NONE;
}

}

Error Parser::parseHeaders(Context& ctx, BufferView& buff, usize& processed) {

	Request& req = ctx.actor.request;
	std::string line;
	Error err;
	bool found;

	while (true) {
		TRY(getChunk(buff, line, processed, found), err);
		if (!found) return ERR_NONE;
		if (line.empty()) {
			TRY(parser::endHeaders(req), err);
			req.has_body = false;
			if (req.chunked) req.has_body = true;
			else if (req.content_length.has_value() and req.content_length.value > 0)
				req.has_body = true;
			ctx.state_ = PROCESSING;
			return ERR_NONE;
		}
		TRY(parser::parseHeaderLine(req, line), err);
	}

	return ERR_NONE;
}


}

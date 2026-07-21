
#include "http/Parser/Parser.hpp"
#include "http/Parser/headers/headers.hpp"
#include "http/Context.hpp"

namespace {

struct Entity {
	usize colon;
	std::string name;
	std::string value;
	std::string normalized;
	http::Error err;
};

static http::Error	prepareHeaderLine(const std::string& line, Entity& entity) {

	entity.colon = line.find(':');
	if (entity.colon == std::string::npos || entity.colon == 0)
		return http::ERR_BAD_REQUEST;

	entity.name = line.substr(0, entity.colon);
	entity.value = line.substr(entity.colon + 1);
	http::parser::trim(entity.name);
	http::parser::trim(entity.value);
	if (entity.name.empty() || entity.value.empty())
		return http::ERR_BAD_REQUEST;

	entity.normalized = http::parser::lowerName(entity.name);
	return http::ERR_NONE;
}

static http::Error	storeHeaderLine(http::Request& request, Entity& entity) {

	entity.err = http::parser::storeHeader(request, entity.name, entity.value);
	if (entity.err != http::ERR_NONE)
		return entity.err;

	return http::parser::handleSpecialHeader(request, entity.normalized, entity.value);
}

static http::Error	parseHeaderLine(http::Request& request, const std::string& line) {

	Entity entity;
	http::Error err;

	err = prepareHeaderLine(line, entity);
	if (err != http::ERR_NONE)
		return err;
	return storeHeaderLine(request, entity);
}

}

namespace http {

Error Parser::parseHeaders(Context& ctx) {

	std::string line;
	Error err;
	bool found;

	while (true) {
		err = getChunk(line, found);
		if (err != ERR_NONE)
			return err;
		if (!found)
			return ERR_NONE;
		if (line.empty()) {
			err = parser::endHeaders(ctx.actor.request);
			if (err != ERR_NONE)
				return err;
			header_bytes = 0;
			ctx.state_ = PROCESSING;
			ctx.action_ = AC_NONE;
			return ERR_NONE;
		}
		err = parseHeaderLine(ctx.actor.request, line);
		if (err != ERR_NONE)
			return err;
	}

	return ERR_NONE;
}

}

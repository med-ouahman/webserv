
#include "http/parser/parse.hpp"
#include "http/parser/headers/headers.hpp"
#include "http/Context.hpp"

namespace {

struct Entity {
	usize colon;
	std::string name;
	std::string value;
	std::string normalized;
	http::Error err;
};

static http::Error	prepare_header_line(const std::string& line, Entity& entity) {

	entity.colon = line.find(':');
	if (entity.colon == std::string::npos || entity.colon == 0)
		return http::ERR_BAD_REQUEST;

	entity.name = line.substr(0, entity.colon);
	entity.value = line.substr(entity.colon + 1);
	http::parser::trim(entity.name);
	http::parser::trim(entity.value);
	if (entity.name.empty() || entity.value.empty())
		return http::ERR_BAD_REQUEST;

	entity.normalized = http::parser::lower_name(entity.name);
	return http::ERR_NONE;
}

static http::Error	store_header_line(http::Request& request, Entity& entity) {

	entity.err = http::parser::store_header(request, entity.name, entity.value);
	if (entity.err != http::ERR_NONE)
		return entity.err;

	return http::parser::handle_special_header(request, entity.normalized, entity.value);
}

static http::Error	parse_header_line(http::Request& request, const std::string& line) {

	Entity entity;
	http::Error err;

	err = prepare_header_line(line, entity);
	if (err != http::ERR_NONE)
		return err;
	return store_header_line(request, entity);
}

}

namespace http {
namespace parser {

static ContextState	next_state(Request& request) {
	if (request.chunked)
		return BODY;
	if (request.content_length.has_value()
		&& request.content_length.value > 0)
		return BODY;
	return PROCESSING;
}

}

Error Context::parse_headers() {

	std::string line;
	Error err;
	bool found;

	while (true) {
		err = get_chunk(line, found);
		if (err != ERR_NONE)
			return err;
		if (!found)
			return ERR_NONE;
		if (line.empty()) {
			err = parser::end_headers(request);
			if (err != ERR_NONE)
				return err;
			header_bytes = 0;
			state_ = parser::next_state(request);
			return ERR_NONE;
		}
		err = parse_header_line(request, line);
		if (err != ERR_NONE)
			return err;
	}

	return ERR_NONE;
}

}

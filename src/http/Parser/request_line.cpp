
#include "http/Parser/Parser.hpp"
#include "http/Context.hpp"

#include <string>

namespace {

static bool	checkSpaces(const std::string& line, usize& first, usize& second) {

	first = line.find(' ');
	if (first == std::string::npos)
		return false;
	second = line.find(' ', first + 1);
	if (second == std::string::npos)
		return false;
	return line.find(' ', second + 1) == std::string::npos;
}

static http::Error	setVersion(http::Request& request, const std::string& line, usize start) {

	usize len = line.size() - start;

	if (len == 8 && line.compare(start, 8, "HTTP/1.0") == 0)
		request.version = http::HTTP_1_0;
	else if (len == 8 && line.compare(start, 8, "HTTP/1.1") == 0)
		request.version = http::HTTP_1_1;
	else if (len >= 5 && line.compare(start, 5, "HTTP/") == 0)
		return http::ERR_UNSUPPORTED_HTTP_VERSION;
	else
		return http::ERR_BAD_REQUEST;

	return http::ERR_NONE;
}

static bool	setTarget(http::Request& request, const std::string& target) {

	usize query_pos;

	if (target.empty() || target[0] != '/')
		return false;

	request.url = target;
	query_pos = target.find('?');
	if (query_pos == std::string::npos) {
		request.path = target;
		request.query = base::Optional<std::string>();
	} else {
		request.path = target.substr(0, query_pos);
		request.query = base::Optional<std::string>(target.substr(query_pos + 1));
	}
	return true;
}

}

namespace http {

Error Parser::parseRequestLine(Context& ctx) {

	std::string line;
	std::string target;

	usize first_space;
	usize second_space;
	bool found;
	Error err;

	err = getChunk(line, found);
	if (err != ERR_NONE)
		return err;
	if (!found)
		return ERR_NONE;
	if (line.empty()) {
		if (leading_crlf) return ERR_BAD_REQUEST;

		leading_crlf = true;
		return ERR_NONE;
	}

	if (!checkSpaces(line, first_space, second_space))
		return ERR_BAD_REQUEST;

	target = line.substr(first_space + 1, second_space - first_space - 1);
	if (first_space == 0 || target.empty() || second_space + 1 >= line.size())
		return ERR_BAD_REQUEST;

	ctx.actor.request.method = methodOf(line.substr(0, first_space));
	if (ctx.actor.request.method == UNKNOWN)
		return ERR_METHOD_NOT_ALLOWED;

	err = setVersion(ctx.actor.request, line, second_space + 1);
	if (err != ERR_NONE)
		return err;
	if (!setTarget(ctx.actor.request, target))
		return ERR_BAD_REQUEST;

	header_bytes = 0;
	phase = PARSING_HEADERS;
	timer.update();
	return ERR_NONE;
}

}

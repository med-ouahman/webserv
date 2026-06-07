
#include "http/parser/parse.hpp"
#include "http/Context.hpp"

#include <string>

namespace {

static bool	check_spaces(const std::string& line, usize& first, usize& second) {

	first = line.find(' ');
	if (first == std::string::npos)
		return false;
	second = line.find(' ', first + 1);
	if (second == std::string::npos)
		return false;
	return line.find(' ', second + 1) == std::string::npos;
}

static bool	set_method(http::Request& request, const std::string& line, usize end) {
	if (end == 3 && line.compare(0, 3, "GET") == 0)
		request.method = http::GET;
	else if (end == 4 && line.compare(0, 4, "POST") == 0)
		request.method = http::POST;
	else if (end == 6 && line.compare(0, 6, "DELETE") == 0)
		request.method = http::DELETE;
	else
		request.method = http::UNKNOWN;
	return true;
}

static http::Error	set_version(http::Request& request, const std::string& line, usize start) {

	usize len = line.size() - start;

	if (len == 8 && line.compare(start, 8, "HTTP/1.0") == 0)
		request.version = http::HTTP_1_0;
	else if (len == 8 && line.compare(start, 8, "HTTP/1.1") == 0)
		request.version = http::HTTP_1_1;
	else if (len >= 5 && line.compare(start, 5, "HTTP/") == 0)
		return http::UNSUPPORTED_HTTP_VERSION;
	else
		return http::EBAD_REQUEST;

	return http::NONE;
}

static bool	set_target(http::Request& request, const std::string& target) {

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

namespace parser {

Error	parse_request_line(Context& ctx) {

	std::string line;
	std::string target;

	usize first_space;
	usize second_space;
	bool found;
	Error err;

	err = get_chunk(ctx, line, found);
	if (err != NONE)
		return err;
	if (!found)
		return NONE;

	if (!check_spaces(line, first_space, second_space))
		return EBAD_REQUEST;

	target = line.substr(first_space + 1, second_space - first_space - 1);
	if (first_space == 0 || target.empty() || second_space + 1 >= line.size())
		return EBAD_REQUEST;

	if (!set_method(ctx.request, line, first_space))
		return EBAD_REQUEST;
	err = set_version(ctx.request, line, second_space + 1);
	if (err != NONE)
		return err;
	if (!set_target(ctx.request, target))
		return EBAD_REQUEST;

	ctx.header_bytes = 0;
	ctx.state_ = HEADERS;
	return NONE;
}

}

}

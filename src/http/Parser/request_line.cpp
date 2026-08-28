
#include "http/Parser/Parser.hpp"
#include "http/Context.hpp"

#include <string>
#include <algorithm>

namespace {

static void setWords(std::string (&words)[4]) {
	std::string::size_type first;
	std::string::size_type second;

	first = words[0].find(' ');
	second = words[0].find(' ', first + 1);
	words[1] = words[0].substr(0, first);
	words[2] = words[0].substr(first + 1, second - first - 1);
	words[3] = words[0].substr(second + 1);
}

static http::Error setMethod(http::Request& request, const std::string& word) {
	if (word.empty())
		return http::ERR_BAD_REQUEST;
	request.method = http::methodOf(word);
	if (request.method == http::UNKNOWN)
		return http::ERR_METHOD_NOT_ALLOWED;
	return http::ERR_NONE;
}

static http::Error setTarget(http::Request& request, const std::string& target) {

	usize query_pos;

	if (target.empty() or target[0] != '/')
		return http::ERR_BAD_REQUEST;

	request.url = target;
	query_pos = target.find('?');
	if (query_pos == std::string::npos) {
		request.path = target;
		request.query = base::Optional<std::string>();
	} else {
		request.path = target.substr(0, query_pos);
		request.query = base::Optional<std::string>(target.substr(query_pos + 1));
	}
	return http::ERR_NONE;
}

static http::Error setVersion(http::Request& request, const std::string& word) {

	const usize len = word.size();

	if (len == 8 and word.compare(0, 8, "HTTP/1.0") == 0)
		request.version = http::HTTP_1_0;
	else if (len == 8 and word.compare(0, 8, "HTTP/1.1") == 0)
		request.version = http::HTTP_1_1;
	else if (len >= 5 and word.compare(0, 5, "HTTP/") == 0)
		return http::ERR_UNSUPPORTED_HTTP_VERSION;
	else
		return http::ERR_BAD_REQUEST;

	return http::ERR_NONE;
}


}

namespace http {

Error Parser::parseRequestLine(Context& ctx, BufferView& buff, usize& processed) {

	Request& req = ctx.actor.request;
	std::string words[4];
	bool found;
	Error err;

	TRY(getChunk(buff, words[0] ,processed ,found), err);
	if (!found) return ERR_NONE;
	if (words[0].empty()) {
		if (leading_crlf) return ERR_BAD_REQUEST;

		leading_crlf = true;
		return ERR_NONE;
	}
	size_t sp_count = std::count(words[0].begin(), words[0].end(), ' ');
	if (sp_count != 2) return ERR_BAD_REQUEST;

	setWords(words);
	TRY(setMethod(req, words[1]), err);
	TRY(setTarget(req, words[2]), err);
	TRY(setVersion(req, words[3]), err);
	header_bytes = 0;
	phase = PARSING_HEADERS;
	timer.update();
	return ERR_NONE;
}

}

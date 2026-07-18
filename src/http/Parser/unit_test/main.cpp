#include "http/Parser/Parser.hpp"
#include "conf.h"

#include <iostream>
#include <string>

static const char* errorName(http::Error err) {
	switch (err) {
		case http::ERR_NONE: return "ERR_NONE";
		case http::ERR_BAD_REQUEST: return "ERR_BAD_REQUEST";
		case http::ERR_UNSUPPORTED_HTTP_VERSION:
			return "ERR_UNSUPPORTED_HTTP_VERSION";
		case http::ERR_MISSING_HOST: return "ERR_MISSING_HOST";
		case http::ERR_DUPLICATE_HEADER: return "ERR_DUPLICATE_HEADER";
		case http::ERR_INVALID_CONTENT_LENGTH:
			return "ERR_INVALID_CONTENT_LENGTH";
		case http::ERR_TE_UNSUPPORTED: return "ERR_TE_UNSUPPORTED";
		case http::ERR_CONFLICTING_BODY_HEADERS:
			return "ERR_CONFLICTING_BODY_HEADERS";
		case http::ERR_HEADER_TOO_LARGE: return "ERR_HEADER_TOO_LARGE";
		case http::ERR_BODY_TOO_LARGE: return "ERR_BODY_TOO_LARGE";
		case http::ERR_REQUEST_TIMEOUT: return "ERR_REQUEST_TIMEOUT";
		case http::ERR_NOT_FOUND: return "ERR_NOT_FOUND";
		case http::ERR_FORBIDDEN: return "ERR_FORBIDDEN";
		case http::ERR_METHOD_NOT_ALLOWED: return "ERR_METHOD_NOT_ALLOWED";
		case http::ERR_LENGTH_REQUIRED: return "ERR_LENGTH_REQUIRED";
		case http::ERR_CGI_FAILED: return "ERR_CGI_FAILED";
		case http::ERR_CGI_TIMEOUT: return "ERR_CGI_TIMEOUT";
		case http::ERR_INTERNAL: return "ERR_INTERNAL";
	}
	return "UNKNOWN_ERROR";
}

int main(void) {
	http::Parser parser;
	std::string line;
	bool found;
	http::Error err;
	usize index;

	parser.phase = http::PARSING_HEADERS;
	parser.raw_buffer = complex_headers;
	index = 0;
	while (true) {
		err = parser.getChunk(line, found);
		std::cout << "chunk[" << index << "]" << std::endl;
		std::cout << "error=" << errorName(err) << std::endl;
		std::cout << "found=" << (found ? "true" : "false") << std::endl;
		std::cout << "content=\"" << line << "\"" << std::endl;
		std::cout << "remaining_size=" << parser.raw_buffer.size()
			<< std::endl << std::endl;
		if (err != http::ERR_NONE || !found || line.empty())
			break;
		++index;
	}
	return err == http::ERR_NONE ? 0 : 1;
}

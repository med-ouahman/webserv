
#include "Parser.hpp"
#include <sstream>

namespace http {

	Parser::ParseResult Parser::parse_request_line() {
		size_t line_index = request_buff.find("\r\n");
		if (line_index == std::string::npos) {
			return NEED_MORE_BYTES;
		}
		std::string req_line = request_buff.substr(0, line_index);
		if (req_line.length() == 0 || req_line == "\r\n") {
			return PARSE_ERROR;
		}
		size_t p1, p2;
		p1 = req_line.find(" ");
		p2 = req_line.find(" ", p1 + 1);
		if (p1 == std::string::npos || p2 == std::string::npos || p2 == req_line.length() - 3) {
			return PARSE_ERROR;
		}
		std::string method = req_line.substr(0, p1);
		request.method = request.get_method(method);
		if (UNKNOWN == request.method) {
			return PARSE_ERROR;
		}
		request.url = req_line.substr(p1 + 1, p2 - p1);
		if (request.url[0] != '/') {
			return PARSE_ERROR;
		}
		request.version = req_line.substr(p2 + 1);
		if (!Parser::validate_http_version(request.version)) {
			std::cout << "Mai\n";
			return PARSE_ERROR;
		}
		parse_state = HEADERS;
		request_buff = request_buff.substr(line_index);
		return SUCCESS;
	}
}


#include "HTTPParser.hpp"
#include <sstream>

namespace http {

	void HTTPParser::parse_request_line( void ) {

		size_t line_index = request_buff.find("\r\n");
		if (line_index == std::string::npos) {
			return ;
		}
		std::string req_line = request_buff.substr(0, line_index);
		request_buff = request_buff.substr(line_index + 2);
		if (req_line.length() == 0 || req_line == "\r\n") {
			parse_state = ERROR;
			return ;
		}
		size_t p1, p2;
		p1 = req_line.find(" ");
		p2 = req_line.find(" ", p1 + 1);
		if (p1 == std::string::npos || p2 == std::string::npos || p2 == req_line.length() - 3) {
			parse_state = ERROR;
			return ;
		}
		std::string method = req_line.substr(0, p1);
		request.method = request.get_method(method);
		if (UNKNOWN == request.method) {
			parse_state = ERROR;
			return ;
		}
		request.url = req_line.substr(p1 + 1, p2 - p1);
		if (request.url[0] != '/') {
			parse_state = ERROR;
			return ;
		}
		request.version = req_line.substr(p2 + 1);
		if (!HTTPParser::validate_http_version(request.version)) {
			parse_state = ERROR;
			return ;
		}
		std::cout << "Request line done\n";
		parse_state = HEADERS;
	}
}

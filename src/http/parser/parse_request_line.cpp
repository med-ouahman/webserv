
#include "Parser.hpp"
#include <iostream>

namespace http {

	base::Expected<RequestLine, int> parser::parse_request_line(const std::string& line) {
		
		RequestLine request_line;
		
		size_t cursor = 0;
		size_t line_offset = 0;
		size_t method_len = 0;

		while (cursor < line.size() and line[cursor] != ' ' and method_len <= http_limits::MAX_METHOD_LEN) {
			++method_len;
			++cursor;
		}
		
		if (cursor == 0 or method_len > http_limits::MAX_METHOD_LEN) {
			return ERROR;
		}

		std::string method = line.substr(line_offset, method_len);
		request_line.method = Request::get_method(method);
		
		if (request_line.method == UNKNOWN) {
			std::cout << "unknown method\n";
			return ERROR;
		}
		++cursor;
		line_offset = cursor;
		size_t uri_len = 0;
		while (cursor < line.size() and line[cursor] != ' ' and uri_len <= http_limits::MAX_URI_LEN) {
			++uri_len;
			++cursor;
		}

		if (cursor == line_offset or uri_len > http_limits::MAX_URI_LEN) {
			return ERROR;
		}

		request_line.uri = line.substr(line_offset, uri_len);
	
		++cursor;
		line_offset = cursor;
		size_t version_len = 0;
		
		while (cursor < line.size() and version_len <= http_limits::MAX_VERSION_LEN) {
			++version_len;
			++cursor;
		}

		if (cursor == line_offset or version_len > http_limits::MAX_VERSION_LEN) {
			return ERROR;
		}

		request_line.version = line.substr(line_offset, version_len);
		if (!validate_http_version(request_line.version))
			return ERROR;

		return request_line;
	}
	
}

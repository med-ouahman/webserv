
#include "HTTPParser.hpp"
#include <sstream>

namespace http {

	Base::Expected<RequestLine, int> HTTPParser::parse_request_line( const std::string& line ) {
		RequestLine request_line;

		size_t cursor = 0;
		size_t line_offset = 0;
		size_t method_len = 0;

		while (cursor < line.size() && line[cursor] != ' ' && method_len <= MAX_METHOD_LEN) {
			++method_len;
			++cursor;
		}
		
		if (cursor == 0 || method_len > MAX_METHOD_LEN) {
			return ERROR;
		}

		std::string method = line.substr(line_offset, method_len);
		request_line.method = HTTPRequestData::get_method(method);
		
		if (request_line.method == UNKNOWN) {
			std::cout << "unknown method\n";
			return ERROR;
		}
		++cursor;
		line_offset = cursor;
		size_t uri_len = 0;
		while (cursor < line.size() && line[cursor] != ' ' && uri_len <= MAX_URI_LEN) {
			++uri_len;
			++cursor;
		}

		if (cursor == line_offset || uri_len > MAX_URI_LEN) {
			return ERROR;
		}

		request_line.uri = line.substr(line_offset, uri_len);
	
		++cursor;
		line_offset = cursor;
		size_t version_len = 0;
		
		while (cursor < line.size() && version_len <= MAX_VERSION_LEN) {
			++version_len;
			++cursor;
		}

		if (cursor == line_offset || version_len > MAX_VERSION_LEN) {
			return ERROR;
		}

		request_line.version = line.substr(line_offset, version_len);
		if (!validate_http_version(request_line.version))
			return ERROR;

		return request_line;
	}
	
}

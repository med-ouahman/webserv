
#include "HTTPParser.hpp"
#include <sstream>

namespace http {

	HTTPParser::ParseResult HTTPParser::parse_request_line( void ) {

		ParseResult scan_res = scan_line(MAX_REQUEST_LINE_LEN);
		if (scan_res != SUCCESS) {
			return scan_res;
		}

		::size_t cursor = 0;
		::size_t line_offset = 0;
		::size_t method_len = 0;
		while (cursor < line_buff.size() && line_buff[cursor] != ' ' && method_len <= MAX_METHOD_LEN) {
			++method_len;
			++cursor;
		}
		
		if (cursor == 0 || method_len > MAX_METHOD_LEN) {
			return PARSE_ERROR;
		}

		std::string method = line_buff.substr(line_offset, method_len);
		request.method = request.get_method(method);
		
		if (request.method == UNKNOWN) {
			return PARSE_ERROR;
		}
		++cursor; // skip the white spae
		line_offset = cursor;
		::size_t uri_len = 0;
		while (cursor < line_buff.size() && line_buff[cursor] != ' ' && uri_len <= MAX_URI_LEN) {
			++uri_len;
			++cursor;
		}
		
		if (cursor == line_offset || uri_len > MAX_URI_LEN) {
			return PARSE_ERROR;
		}

		request.url = line_buff.substr(line_offset, uri_len);
	
		++cursor;
		line_offset = cursor;
		::size_t version_len = 0;

		while (cursor < line_buff.size() && version_len <= MAX_VERSION_LEN) {
			++version_len;
			++cursor;
		}

		if (cursor == line_offset || version_len > MAX_VERSION_LEN) {
			
			return PARSE_ERROR;
		}
		
		request.version = line_buff.substr(line_offset, version_len);
		line_buff.clear();

		parse_state = HEADERS;
		return SUCCESS;
	}
	
}


#include "HTTPParser.hpp"
#include <sstream>

namespace http {

	HTTPParser::ParseResult::Type HTTPParser::parse_request_line( void ) {
				
		while (true) {
			ParseResult::Type scan_res = scan_line(MAX_REQUEST_LINE_LEN);
			if (scan_res != ParseResult::SUCCESS) {
				return scan_res;
			}
			ticks_since_progress = 0;
			if (line_buff.empty() && leading_crlf < MAX_LEADING_CRLF) {
				leading_crlf++;
				continue;
			} else {
				break;
			}
		}

		::size_t cursor = 0;
		::size_t line_offset = 0;
		::size_t method_len = 0;
		while (cursor < line_buff.size() && line_buff[cursor] != ' ' && method_len <= MAX_METHOD_LEN) {
			++method_len;
			++cursor;
		}
		
		if (cursor == 0 || method_len > MAX_METHOD_LEN) {
			return ParseResult::PARSE_ERROR;
		}

		std::string method = line_buff.substr(line_offset, method_len);
		request.method = request.get_method(method);
		
		if (request.method == UNKNOWN) {
			std::cout << "unknown method\n";
			return ParseResult::PARSE_ERROR;
		}
		++cursor;
		line_offset = cursor;
		::size_t uri_len = 0;
		while (cursor < line_buff.size() && line_buff[cursor] != ' ' && uri_len <= MAX_URI_LEN) {
			++uri_len;
			++cursor;
		}

		if (cursor == line_offset || uri_len > MAX_URI_LEN) {
			return ParseResult::PARSE_ERROR;
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
			return ParseResult::PARSE_ERROR;
		}
		
		request.version = line_buff.substr(line_offset, version_len);
		line_buff.clear();
		parse_state = ParseState::HEADERS;
		return ParseResult::SUCCESS;
	}
	
}

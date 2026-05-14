
#include "HTTPParser.hpp"
#include <sstream>

namespace http {

	ScanResult HTTPParser::parse_request_line() {
				
		while (true) {
			ScanResult scan_res = line_c.scan(MAX_REQUEST_LINE_LEN);
		
			if (scan_res != SUCCESS) {
				return scan_res;
			}

			if (line_c.line().empty() && leading_crlf_count < MAX_LEADING_CRLF) {
				leading_crlf_count++;
				continue;
			} else {
				break;
			}
		}

		size_t cursor = 0;
		size_t line_offset = 0;
		size_t method_len = 0;

		while (cursor < line_c.line().size() && line_c.line()[cursor] != ' ' && method_len <= MAX_METHOD_LEN) {
			++method_len;
			++cursor;
		}
		
		if (cursor == 0 || method_len > MAX_METHOD_LEN) {
			return ERROR;
		}

		std::string method = line_c.line().substr(line_offset, method_len);
		request.method = request.get_method(method);
		
		if (request.method == UNKNOWN) {
			std::cout << "unknown method\n";
			return ERROR;
		}
		++cursor;
		line_offset = cursor;
		size_t uri_len = 0;
		while (cursor < line_c.line().size() && line_c.line()[cursor] != ' ' && uri_len <= MAX_URI_LEN) {
			++uri_len;
			++cursor;
		}

		if (cursor == line_offset || uri_len > MAX_URI_LEN) {
			return ERROR;
		}

		request.url = line_c.line().substr(line_offset, uri_len);
	
		++cursor;
		line_offset = cursor;
		size_t version_len = 0;
		while (cursor < line_c.line().size() && version_len <= MAX_VERSION_LEN) {
			++version_len;
			++cursor;
		}
		if (cursor == line_offset || version_len > MAX_VERSION_LEN) {
			return ERROR;
		}

		request.version = line_c.line().substr(line_offset, version_len);
		if (!validate_http_version(request.version))
			return ERROR;
			
		line_c.reset();
		parse_state = ParseState::HEADERS;
		
		return SUCCESS;
	}
	
}

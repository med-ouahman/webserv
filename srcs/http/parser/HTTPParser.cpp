#include "HTTPParser.hpp"

namespace http {

	HTTPParser::HTTPParser(): header_bytes_parsed(0), headers_done(false) {
		parse_state = REQUEST_LINE;
	}

	HTTPParser::~HTTPParser() {

	}

	HTTPParser::ParseResult HTTPParser::consume( char* buff ) {
		
		request_buff.append(buff);
		buff[0] = 0;
		
		switch (parse_state) {
			case REQUEST_LINE:
				parse_request_line();
				break;
			case HEADERS:
				parse_headers();
				break;
			case BODY:
				parse_body();
				break;
			default:
				break;
		}
		if (parse_state == DONE) {
			return SUCCESS;
		}
		if (parse_state == ERROR) {
			return PARSE_ERROR;
		}
		if (request_buff.size() != 0) {	
			return CONTINUE;
		}
		if (headers_done && request.body_len == 0) {
			return SUCCESS;
		}
		return NEED_MORE_BYTES;
	}

	HTTPRequest HTTPParser::get_request() const {
		return request;
	}

	void HTTPParser::reset( void ) {
		parse_state = REQUEST_LINE;
		request = HTTPRequest();
	}
}

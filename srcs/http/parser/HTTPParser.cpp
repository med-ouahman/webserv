#include "HTTPParser.hpp"

namespace http {

	HTTPParser::HTTPParser() {
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
			request = HTTPRequest();
			return SUCCESS;
		}
		if (parse_state == ERROR)
			return PARSE_ERROR;
		return NEED_MORE_BYTES;
	}

	HTTPRequest HTTPParser::get_request() const {
		return request;
	}
}

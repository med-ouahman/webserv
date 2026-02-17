#include "Parser.hpp"

namespace http {

	Parser::Parser() {
		parse_state = REQUEST_LINE;
	}

	Parser::~Parser() {

	}

	Parser::ParseResult Parser::consume( const char* buff ) {
		request_buff.append(buff);

		ParseResult parse_result;
		switch (parse_state) {
			case REQUEST_LINE:
				parse_result = parse_request_line();
				if (parse_result == SUCCESS) std::cout << "Success\n";
				else std::cout << "Failure\n";
				break;
			case HEADERS:
				parse_result = parse_headers();
				break;
			case BODY:
				parse_result = parse_body();
				break;
			default:
				parse_result = PARSE_ERROR;
		}
		return parse_result;
	}

	HTTPRequest Parser::get_request() const {
		return request;
	}
}

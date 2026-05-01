#include "HTTPParser.hpp"

namespace http {
    HTTPParser::ParseResult::Type HTTPParser::parse( void ) {
		
		ParseResult::Type r;
	
		if (ParseState::REQUEST_LINE == parse_state) {
			r = parse_request_line();
		}

		if (ParseState::HEADERS == parse_state) {
			r = parse_headers();
		}

		if (ParseState::BODY == parse_state) {
			r = parse_body();
		}

		if (ParseState::ERROR == parse_state) {
			return ParseResult::PARSE_ERROR;
		}
		
		return r;
	}
}
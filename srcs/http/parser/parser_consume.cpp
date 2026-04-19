#include "HTTPParser.hpp"

namespace http {
    HTTPParser::ParseResult::Type HTTPParser::consume( void ) {
		
		ParseResult::Type r;
	
		ticks_since_progress++;
		if (ParseState::REQUEST_LINE == parse_state) {
			current_state_tick_limit = REQUEST_LINE_LIMIT_TICKS;
			r = parse_request_line();
		}

		if (ParseState::HEADERS == parse_state) {
			current_state_tick_limit = HEADERS_LIMIT_TICKS;
			r = parse_headers();
		}

		if (ParseState::BODY == parse_state) {
			current_state_tick_limit = BODY_LIMIT_TICKS;
			r = parse_body();
		}

		if (ParseState::ERROR == parse_state) {
			return ParseResult::PARSE_ERROR;
		}
		
		if (ParseResult::NEED_MORE_BYTES == r) {
			bytes_consumed = 0;
		}
		
		if (ticks_since_progress > current_state_tick_limit) {
			std::cout << "ticks: " << ticks_since_progress << "\n";
			return ParseResult::TIMEOUT;
		}
		return r;
	}
}
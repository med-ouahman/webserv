#include "HTTPParser.hpp"

namespace http {
	
    ScanResult HTTPParser::parse() {
		
		ScanResult r;
	
		if (ParseState::REQUEST_LINE == parse_state) {
			r = parse_request_line();
		}

		if (ParseState::HEADERS == parse_state) {
			r = parse_headers();
		}

		if (ParseState::ERROR == parse_state) {
			return ERROR;
		}
		
		return r;
	}
}
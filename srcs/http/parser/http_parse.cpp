#include "HTTPParser.hpp"

namespace http {
	
    ScanResult HTTPParser::parse() {
		
		ScanResult r;
	
		if (headers_done)
			return SUCCESS;
	
		if (ParseState::REQUEST_LINE == parse_state) {
			r = parse_request_line();
		} else if (ParseState::HEADERS == parse_state) {
			r = parse_headers();
		}

		if (ParseState::ERROR == parse_state) {
			return ERROR;
		}
		std::cout << r << "\n";
		return r;
	}
}

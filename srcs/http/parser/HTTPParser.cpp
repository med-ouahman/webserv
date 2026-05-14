#include "HTTPParser.hpp"

namespace http {
	
	HTTPParser::HTTPParser( DataView& v )
		: headers_done(false),
		
		leading_crlf_count(0),
		header_count(0),
		line_c(v),
		parse_state(ParseState::REQUEST_LINE) {}

	HTTPParser::~HTTPParser() {}


 	HTTPRequest& HTTPParser::get_request() {
		return request;
	}

	void HTTPParser::reset() {
		header_count = 0;
		leading_crlf_count = 0;
		headers_done = false;
		parse_state = ParseState::REQUEST_LINE;
		request = HTTPRequest();
	}

	ParseState::Type HTTPParser::get_parser_state() const {
		return parse_state;
	}
}


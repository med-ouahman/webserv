#include "HTTPParser.hpp"

namespace http {
	
	HTTPParser::HTTPParser( const config::Config& conf )
		: headers_done(false),
		
		leading_crlf_count(0),
		header_count(0),
		parse_state(ParseState::REQUEST_LINE),
		config(conf) {}

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

	void HTTPParser::set_data_view( core::DataView* view ) {

		line_c.set_data_view(view);
	}

	ParseState::Type HTTPParser::get_parser_state() const {
		return parse_state;
	}
}


#include "HTTPParser.hpp"

namespace http {

	
	HTTPParser::HTTPParser( int connection_fd )
		:conn_fd(connection_fd),
		body_dir("./srcs/http/parser/.body_dir"),
		body_bytes_parsed(0),
		headers_done(false),
		header_count(0),
		bytes_consumed(0),
		data_(NULL),
		len_(0),
		parse_state(ParseState::REQUEST_LINE) {}

	HTTPParser::~HTTPParser() {}

	HTTPParser::ParseResult::Type HTTPParser::consume( const char* buff, ::size_t size ) {
		
		ParseResult::Type r;
		data_ = (char*)buff;
		len_ = size;
		bytes_consumed = 0;

		if (ParseState::REQUEST_LINE == parse_state) {
			r = parse_request_line(); // can muate parse_state
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

	HTTPRequest HTTPParser::get_request() const {
		return request;
	}

	void HTTPParser::reset( void ) {
		parse_state = ParseState::REQUEST_LINE;
		request = HTTPRequest();
	}

	HTTPParser::ParseResult::Type HTTPParser::scan_line( ::size_t max_bytes_allowed ) {
		::size_t line_offset = line_buff.size();
		::size_t i = bytes_consumed;
		bool cr_found = false;
		bool nl_found = false;
		if (i == len_) {
			return ParseResult::NEED_MORE_BYTES;
		}
		while (i < len_) {
			
			if (line_offset >= max_bytes_allowed) {
				return ParseResult::PARSE_ERROR;
			}

			line_offset++;

			if (data_[i] == '\r') {
				cr_found = true;
				++i;
			} else {
				if (data_[i] == '\n' && cr_found) {
					++i;
					nl_found = true;
					break;
				}
				++i;
			}
		}

		size_t to_append = i - 2 - bytes_consumed;
		std::cout << "append: " << to_append << '\n';
		line_buff.append(data_ + bytes_consumed, to_append); // don't store \r\n
		bytes_consumed = i;

		if (!nl_found) {
			return ParseResult::NEED_MORE_BYTES;
		}
		
		return ParseResult::SUCCESS;
	}

}


#include "HTTPParser.hpp"

namespace http {

	HTTPParser::HTTPParser()
		:headers_done(false),
		header_count(0),
		bytes_consumed(0),
		data_(NULL),
		len_(0),
		parse_state(REQUEST_LINE) {}

	HTTPParser::~HTTPParser() {}

	HTTPParser::ParseResult HTTPParser::consume( const char* buff, ::size_t size ) {
		
		ParseResult r;
		data_ = (char*)buff;
		len_ = size;
		bytes_consumed = 0;
		if (REQUEST_LINE == parse_state) {
			r = parse_request_line();
		}
		if (HEADERS == parse_state) {
			r = parse_headers();
		}
		if (BODY == parse_state) {
			r = parse_body();
		}
		if (ERROR == parse_state) {
			return PARSE_ERROR;
		}
		return r;
	}

	HTTPRequest HTTPParser::get_request() const {
		return request;
	}

	void HTTPParser::reset( void ) {
		parse_state = REQUEST_LINE;
		request = HTTPRequest();
	}

	HTTPParser::ParseResult HTTPParser::scan_line( ::size_t max_bytes_allowed ) {
		::size_t line_offset = line_buff.size();
		::size_t i = bytes_consumed;
		bool cr_found = false;
		bool nl_found = false;
		if (i == len_) {
			return NEED_MORE_BYTES;
		}
		while (i < len_) {
			
			if (line_offset >= max_bytes_allowed) {
				return PARSE_ERROR;
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
			return NEED_MORE_BYTES;
		}
		
		return SUCCESS;
	}

}


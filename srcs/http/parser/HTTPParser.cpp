#include "HTTPParser.hpp"

namespace http {

	const std::string HTTPParser::hexas = "0123456789abcdef";
	
	HTTPParser::HTTPParser( int connection_fd )
		:conn_fd(connection_fd),
		body_dir("./srcs/http/parser/.body_dir"),
		body_bytes_parsed(0),
		body_len(0),
		body_path(""),
		body_fd(-1),
		chunk_state(ChunkState::CHUNK_SIZE),
		chunk_remaining(0),
		headers_done(false),
		cr_found(false),
		leading_crlf(0),
		header_count(0),
		bytes_consumed(0),
		data_(NULL),
		len_(0),
		parse_state(ParseState::REQUEST_LINE),
		body_type(BodyType::UNSET) {}

	HTTPParser::~HTTPParser() {}

	HTTPParser::ParseResult::Type HTTPParser::consume( const char* buff, ::size_t size ) {
		
		ParseResult::Type r;
		if (bytes_consumed == len_) {
			data_ = (char*)buff;
			len_ = size;
			bytes_consumed = 0;
		}
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

	HTTPRequest HTTPParser::get_request() const {
		return request;
	}

	void HTTPParser::reset( void ) {
		header_count = 0;
		leading_crlf = 0;
		headers_done = false;
		cr_found = false;
		body_type = BodyType::UNSET;
		body_len = 0;
		body_bytes_parsed = 0;
		body_fd = -1;
		parse_state = ParseState::REQUEST_LINE;
		request = HTTPRequest();
	}

	HTTPParser::ParseResult::Type HTTPParser::scan_line( ::size_t max_bytes_allowed ) {
		::size_t line_offset = line_buff.size();
		::size_t i = bytes_consumed;
		std::cout << "=====================\n";
		std::cout << "size: " << len_ << "\n";
		// std::cout << data_ << "\n";
		std::cout << "=====================\n";
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
				if (data_[i] == '\n' && cr_found && data_[i - 1] == '\r') {
					++i;
					nl_found = true;
					cr_found = false;
					break;
				}
				++i;
			}
		}
		size_t to_append = i - bytes_consumed - 1 * nl_found;
		line_buff.append(data_ + bytes_consumed, to_append);
		bytes_consumed = i;
		std::cout << (line_buff == "\r\n"?"yes\n":"no\n");
		if (!nl_found) {
			return ParseResult::NEED_MORE_BYTES;
		}
		line_buff.erase(line_buff.size() - 1, 1);
		return ParseResult::SUCCESS;
	}

}


#include "HTTPParser.hpp"

namespace http {

	const std::string HTTPParser::hexas = "0123456789abcdef";
	
	HTTPParser::HTTPParser( int connection_fd, const config::Config& conf )
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
		leading_crlf_count(0),
		header_count(0),
		bytes_consumed(0),
		data_(NULL),
		len_(0),
		parse_state(ParseState::REQUEST_LINE),
		body_type(BodyType::UNSET), 
		config(conf) {}

	HTTPParser::~HTTPParser() {}


	HTTPRequest HTTPParser::get_request() const {
		return request;
	}

	void HTTPParser::reset( void ) {
		header_count = 0;
		leading_crlf_count = 0;
		headers_done = false;
		cr_found = false;
		body_type = BodyType::UNSET;
		body_len = 0;
		body_bytes_parsed = 0;
		body_fd = -1;
		parse_state = ParseState::REQUEST_LINE;
		request = HTTPRequest();
	}

	void HTTPParser::feed( const char* buff, ::size_t size ) {
		if (bytes_consumed == len_) {
			data_ = (char *)buff;
			len_ = size;
			bytes_consumed = 0;
		}
	}

	std::size_t HTTPParser::get_bytes_consumed( void ) const {
		return bytes_consumed;
	}

	HTTPParser::ParseState::Type HTTPParser::get_parser_state( void ) const {
		return parse_state;
	}

	::size_t HTTPParser::get_body_bytes_consumed( void ) const {
		return body_bytes_parsed;
	}
}


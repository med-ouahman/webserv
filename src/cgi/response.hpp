#pragma once

#include "LineScanner.hpp"
#include "http/common/Headers.hpp"

namespace cgi {

enum CGIParseState {
	STATUS_LINE,
	HEADERS,
	HEADERS_DONE,
	CGI_ERROR
};

enum ParseResult {
	PARSE_SUCCESS,
	PARSE_ERROR,
	PARSE_CONTINUE,
};


struct CGIParseContext {
	static const std::size_t MAX_CGI_HEADER_BLOCK_LEN = 4096;
	http::LineScanner 	sc_;
	size_t			header_bytes_;
	CGIParseState 	state_;
};

class CGIResponseBuilder {
private:
    http::Headers headers_;
    CGIParseContext ctx_;

public:
	ParseResult parse_headers( DataView& view );
	void sanitize_status_line( const std::pair<std::string, std::string>& header );
	void sanitize_header( std::pair<std::string, std::string>& header );

};

}
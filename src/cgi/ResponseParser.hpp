#pragma once

#include "http/common/Headers.hpp"
#include "LineScanner.hpp"

namespace cgi {
namespace parser {

enum CGIParseState {
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

class ResponseParser {
private:
    http::Headers headers_;
    CGIParseContext parse_ctx;

public:
	ParseResult parse_headers( DataView& view );
	void sanitize_status_line( const std::pair<std::string, std::string>& header );
	void sanitize_header( std::pair<std::string, std::string>& header );
    const http::Headers& headers() const;
    bool finished() const;
};

}
}
#pragma once

#include "http/common/Headers.hpp"
#include "LineScanner.hpp"

namespace http {

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
	LineScanner 	sc_;
	size_t			header_bytes_;
	CGIParseState 	state_;
};

class ResponseParser {
private:
    Headers headers_;
    CGIParseContext parse_ctx;

public:
	ParseResult parse_headers( DataView& view );
	void sanitize_status_line( const std::pair<std::string, std::string>& header );
	void sanitize_header( std::pair<std::string, std::string>& header );
    const Headers& headers() const;
    bool finished() const;
};

}

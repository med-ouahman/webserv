#pragma once

#include "CStringArray.hpp"
#include "UniqueFd.hpp"
#include "Request.hpp"
#include "http/common/Headers.hpp"
#include "LineScanner.hpp"

namespace http { struct ResolutionResult; }

namespace cgi {

namespace resolver {

struct CGIRequestContext {
    std::string request_method;
    std::string mime_type;
    std::string interpreter;
    std::string script_name;
    std::string query_string;
    std::string working_dir;
    std::string body_filename;

    size_t		timeout_seconds;
    size_t      body_content_length;

    std::string path_info;
    std::string server_name;
    uint16_t    server_port;
};

struct CGIExecContext {
    std::string interpreter;
    std::string working_dir;
    
    UniqueFd  stdin_fd;
    size_t    timeout_seconds;

    CStringArray argv;
    CStringArray envp;

};

struct Context {
    CGIRequestContext req_;
    CGIExecContext exec_;

    Context() {

    }
    
    Context( Context& c ) {
        int fd = c.exec_.stdin_fd.release();
        this->exec_.stdin_fd.set(fd);
    }
};

Context resolve(http::Request const& req, const http::ResolutionResult& r);

}

namespace envp {

struct CGIRequestContext;

class Builder {

private:
    const static std::string http_prefix;
    const static char* metadata[];
	const static char* stripped_headers[];
    static std::string      transform( bool has_http_prefix, http::Headers::const_iterator& it );
	static http::Headers 	build_metadata( const resolver::CGIRequestContext& context );
	static bool             forbidden_header( const std::string& header_name );

public:
    static CStringArray build( const resolver::CGIRequestContext& ctx,
       http::Headers const& request_headers );
};

}

namespace parser {

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

class ResponseBuilder {
private:
    http::Headers headers_;
    CGIParseContext ctx_;

public:
	ParseResult parse_headers( DataView& view );
	void sanitize_status_line( const std::pair<std::string, std::string>& header );
	void sanitize_header( std::pair<std::string, std::string>& header );
    const http::Headers& headers() const;

};

}
}
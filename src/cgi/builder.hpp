#pragma once

#include "CStringArray.hpp"
#include "http/common/Headers.hpp"

namespace cgi {

struct CGIRequestContext;

class CGIEnvBuilder {

private:
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
#pragma once

#include "CStringArray.hpp"
#include "http/common/Headers.hpp"

namespace cgi {

struct CGIRequestContext;

class CGIEnvBuilder {

private:
    const static char* cgi_metadata[];
	const static char* stripped_headers[];
    char* 		transform( bool http_prefix, http::Headers::const_iterator& it );
	http::Headers 	build_metadata( const CGIRequestContext& context );
	static bool forbidden_header( const std::string& header_name );

public:
    CStringArray build();
};

}
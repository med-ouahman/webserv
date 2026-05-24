#pragma once

#include "CStringArray.hpp"
#include "UniqueFd.hpp"

namespace cgi {

struct ResolutionResult;

struct CGIRequestContext {

	std::string interpreter;
	std::string working_dir;
	std::string body_filename;
	size_t		timeout_seconds;
};

struct CGIExecContext {
    std::string interpreter;
    std::string working_dir;
    
    UniqueFd  stdin_fd;
    size_t    timeout_seconds;

    CStringArray argv;
    CStringArray envp;

    CGIExecContext() {}
};

class CGIResolver {

public:
	static CGIRequestContext request_context();
	static CGIExecContext	 execution_context();
};

}

#pragma once

#include <string>

#include "IRequestHandler.hpp"
#include "base/base.hpp"
#include "config/Config.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "http/Parser/parser.hpp"

#define CRLF "\r\n"

namespace http {

class Context;

enum ContextState {
	REQUEST_LINE,
	HEADERS,
	PROCESSING,
	CGI_RUNNING,
	RESPONSE_READY,
	WRITING_RESPONSE,
	DONE,
	ERROR,
};

enum ContextAction {
	AC_READ,
	AC_WORK,
	AC_WRITE,
	AC_CLOSE
};

class Context {

private:

	ParserState parser;
	Request		request;
	Response	response;

	ContextState	state_;
	ContextAction	action_;
	
	IRequestHandler* handler;
	friend struct ParserState;

public:

	Context();
	Context(usize conn_id, usize request_id);

	Error consume(const char* data, usize size);
	Error process(const config::Config& config);
	Error produce(base::io::Writer& writer);

	ContextAction next_action() const;
};

}

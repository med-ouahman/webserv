
#pragma once

#include <string>

#include "base/base.hpp"
#include "config/Config.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "http/Parser/parser.hpp"
#include "HandlerFactory.hpp"

#define CRLF "\r\n"

class BufferReader;

namespace http {

struct ResolutionResult {};
struct CGIResult;
class IRequestHandler;

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
	AC_CLOSE,
};

class Context {

private:
	ParserState parser;
	Request		request;
	Response	response;

	ContextState	state_;
	ContextAction	action_;
	
	friend struct ParserState;

	IRequestHandler* handler;
	HandlerFactory factory;

public:

	Context(ServerContext& serv_ctx);
	~Context();

	Error consume(const char* data, usize size);
	Error process(const config::Config& config);
	Error produce(base::io::Writer& writer);

	Error produce(BufferWriter& writer);

	void on_cgi_ready();

	ContextAction next_action() const;
};

}

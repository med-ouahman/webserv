
#pragma once

#include "base/base.hpp"
#include "config/Config.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "http/Parser/Parser.hpp"
#include "http/routing/Routing.hpp"

#define CRLF "\r\n"
#define HTTP_SERVER_ROOT "server/root"

class BufferReader;

namespace http {

class Context;
class ErrorHandler;
class RequestHandler;
struct CGIRequestContext;
struct CGIExecContext;

enum ContextState {
	PARSING,
	PROCESSING,
	DONE,
	ERROR,
};

enum ContextAction {
	AC_READ,
	AC_WRITE,
	AC_CLOSE
};

class Context {

private:

	Parser parser;
	Request request;
	Response response;
	std::string response_head_;

	base::Optional<Decision> route;
	RequestHandler* handler_;

	usize conn_id_;
	usize request_id_;
	usize response_head_offset_;
	usize response_body_offset_;
	Error error_;

	ContextState	state_;
	ContextAction	action_;
	bool			response_started_;

	Context(const Context&);
	Context& operator=(const Context&);
	void responseReady();

	Error setError(Error error);
	Error routeRequest(const config::Config& config);
	Error readBody();
	Error createHandler();
	Error handleError();
	Error writeResponse(base::io::Writer& writer, usize& sent);
	Error writeResponseHead(base::io::Writer& writer, usize& sent);
	Error writeResponseBody(base::io::Writer& writer, usize& sent);
	usize handleResponseFailure(Error err);

	void process(const config::Config& config);


	friend class Parser;
	friend class RequestHandler;
	friend class ErrorHandler;
	friend Error buildCGIContext(const Context& context,
		const config::Config& config,
		CGIRequestContext& request_ctx,
		CGIExecContext& exec_ctx);


public:

	Context();
	Context(usize conn_id, usize request_id);
	~Context();

	usize consume(const config::Config& config, const char* data, usize size);
	usize produce(char *buffer, usize size);

	ContextAction nextAction() const;
	bool timedOut();
};

}

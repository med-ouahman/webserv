
#pragma once

#include "base/base.hpp"
#include "config/Config.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "http/Parser/Parser.hpp"
#include "http/routing/Routing.hpp"
#include "cgi/CGIContext.hpp"

#define CRLF "\r\n"
#define HTTP_SERVER_ROOT "server/root"

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

struct Actor {
	Parser parser;
	Request request;
	Response response;
	RequestHandler* handler;

	Actor();
	void reset();
};

struct Info {
	const std::vector<const config::ServerConfig*>& servers;
	usize conn_id;
	usize request_id;
	base::Optional<DispatchInfo> dispatch;

	Info(const std::vector<const config::ServerConfig*>& servers,
		usize conn_id, usize request_id);
};

struct RequestCount {
	usize active_cgi;
	usize active_requests;

	RequestCount();
};

class Context {

private:

	Actor actor;
	Info info;
	Error error_;

	ContextState	state_;
	ContextAction	action_;

	static RequestCount request_count;

	Context(const Context&);
	Context& operator=(const Context&);

	void responseReady();
	void resetCycle();

	Error setError(Error error);
	Error resolveDispatch();
	Error prepareBodyStorage();
	Error readBody();

	Error createHandler();
	Error handleError();

	usize handleResponseFailure(Error err);

	void process();


	friend class Parser;
	friend class RequestHandler;
	friend class ErrorHandler;
	friend class CGIHandler;
	friend Error buildCGIContext(const Context& context,
		CGIRequestContext& request_ctx,
		ProcessContext& exec_ctx);

public:

	Context(const std::vector<const config::ServerConfig*>& servers,
		usize conn_id, usize request_id);
	~Context();

	usize consume(const char* data, usize size);
	usize produce(char *buffer, usize size);

	ContextAction nextAction() const;
	bool reconcile();
};

}

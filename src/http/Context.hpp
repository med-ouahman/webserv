
#pragma once

#include "base/base.hpp"
#include "config/Config.hpp"

#include "http/Request.hpp"
#include "http/Response.hpp"
#include "http/Parser/Parser.hpp"
#include "http/routing/Routing.hpp"

#include "cgi/CGIContext.hpp"
#include "server/RuntimeServices.hpp"

#include "http/Error.hpp"

#define CRLF "\r\n"
#define HTTP_SERVER_ROOT "server/root"

namespace http {

class Context;
class ErrorHandler;
class ARequestHandler;
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
	AC_CLOSE,
	AC_NONE
};

struct Actor {
	Parser parser;
	Request request;
	Response response;
	ARequestHandler* handler;

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

class Context {

private:

	Actor actor_;
	Info info_;
	Error error_;

	ContextState	state_;

	static usize active_cgi;
	usize active_requests;

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

	friend class Parser;
	friend class ARequestHandler;
	friend class ErrorHandler;
	friend class CgiHandler;

	friend Error cgi::buildCGIContext(const Context& context,
		cgi::CGIRequestContext& request_ctx,
		cgi::ProcessContext& exec_ctx);


public:
	RuntimeServices& services_;
	ContextAction action_;
	
	Context(const std::vector<const config::ServerConfig*>& servers,
		usize conn_id, usize request_id, RuntimeServices& services);
	~Context();

	usize consume(const char* data, usize size);
	void process();
	usize produce(char *buffer, usize size);

	ContextAction nextAction() const;

	bool timeout();
};

}

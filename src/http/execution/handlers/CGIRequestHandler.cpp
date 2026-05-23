#include <signal.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include "Timestamp.hpp"
#include "CGIRequestHandler.hpp"
#include "Dispatcher.hpp"
#include "BodyProvider.hpp"

namespace http {

    const char* CGIRequestHandler::cgi_metadata[] = {"REQUEST_METHOD", "SERVER_PROTOCOL", "QUERY_STRING", NULL};

    const char* CGIRequestHandler::stripped_headers[] = {"transfer-encoding", "content-length", "content-type", "connection", NULL};

    CGIRequestHandler::CGIRequestHandler(  const ResolutionResult res_ )
        : result(res_),
        process(create_exec_context()),
        stdout_(process.stdout_pipe().read_end().get(), io::READABLE),
        stderr_(process.stderr_pipe().read_end().get(), io::READABLE) {

        }

    CGIRequestHandler::~CGIRequestHandler() {
        
    }

    void CGIRequestHandler::handle() {
    
    }

    bool CGIRequestHandler::done() {
        return true;
    }

    void CGIRequestHandler::consume( DataView& view ) {
        parse_headers(view);
    }

    void CGIRequestHandler::produce( BufferWriter& w ) {
        // read body from the request body
    }

    void CGIRequestHandler::on_stream_error() {
        // close
    }

    void CGIRequestHandler::on_stream_closed() {
        // finished;
    }


    cgi::CGIExecContext CGIRequestHandler::create_exec_context() {

        cgi::CGIExecContext ctx;

        CGIRequestContext request_context = Dispatcher::resolve_cgi_context(result);

        ctx.interpreter_path = request_context.interpreter_path;

        ctx.working_dir = request_context.working_directory;

        ctx.stdin_fd = Fd(open(request_context.temp_body_path.c_str(), O_RDONLY));
        
        ctx.timeout_seconds = request_context.timeout_seconds;

        ctx.argv.push(ctx.interpreter_path.c_str());

        ctx.envp = build_envp(request_context);

        return ctx; 
    }
}
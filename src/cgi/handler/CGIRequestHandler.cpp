#include "Timestamp.hpp"
#include "CGIRequestHandler.hpp"
#include "Dispatcher.hpp"
#include "Request.hpp"
#include "EnvBuilder.hpp"
#include "CGIBodyProvider.hpp"

namespace http {

CGIRequestHandler::CGIRequestHandler(const ResolutionResult& res, http::Request const& req, CGIControl& ctl)
    : process(cgi::resolve_exec_context(req, res)),
    stdin_stream(process.stdin_pipe().write_end(), io::WRITABLE, *this),
    stdout_stream(process.stdout_pipe().read_end(), io::READABLE, *this),
    stderr_stream(process.stderr_pipe().read_end(), io::READABLE, *this),
    cgi_ctl(ctl) {

    if (!process.running()) {
        state_ = ERROR;
        return;
    }

    RegisterContext& registrar = cgi_ctl.register_ctx;
    registrar.callback(&stdin_stream, registrar.registrar);
    registrar.callback(&stdout_stream, registrar.registrar);
    registrar.callback(&stderr_stream, registrar.registrar);
}

CGIRequestHandler::~CGIRequestHandler() {}

void CGIRequestHandler::handle() {}

bool CGIRequestHandler::done() {
    return state_ == RESPONSE_DONE;
}

void CGIRequestHandler::consume(BufferReader& reader) {
    if (state_ == BUILDING_HEADERS) {
        builder.parse_headers(reader);
        if (builder.finished()) state_ = HEADERS_READY;
    }
    
    if (state_ == HEADERS_READY) {
        result_.headers = builder.headers();
        result_.code = builder.status_code();
        result_.status_reason = "";
        result_.body = new CGIBodyProvider(reader);
        state_ = STREAMING_BODY;
    }
    
}

void CGIRequestHandler::produce(BufferWriter& w) {
    // read body from the request body
    std::string body = "Hello world\n";
    w.write(body.c_str(), body.size());
}

void CGIRequestHandler::on_stream_error() {
    // close
}

void CGIRequestHandler::on_stream_closed() {
    state_ = RESPONSE_DONE;
    CGICOutputallback callback = cgi_ctl.output_ctx.cb_;
    callback(cgi_ctl.output_ctx.ctx);
}

CGIRequestHandler::State CGIRequestHandler::state() const {
    return state_;
}


CGIResult& CGIRequestHandler::result() {
    return result_;
}
}

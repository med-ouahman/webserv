#include <signal.h>
#include <sys/wait.h>
#include "Timestamp.hpp"
#include "CGIRequestHandler.hpp"
#include "Dispatcher.hpp"
#include "Request.hpp"
#include "EnvBuilder.hpp"

namespace http {
CGIRequestHandler::CGIRequestHandler( const ResolutionResult& res, http::Request const& req)
    : process_(cgi::resolve_exec_context(req, res)),
    stdin_(process_.stdin_pipe().write_end(), io::WRITABLE, *this),
    stdout_(process_.stdout_pipe().read_end(), io::READABLE, *this),
    stderr_(process_.stderr_pipe().read_end(), io::READABLE, *this) {

}

CGIRequestHandler::~CGIRequestHandler() {}

void CGIRequestHandler::handle() {
    std::cout << "Handling CGI\n";
    /* PLACE HOLDER TO SILENCE THE COMPILER BECAUSE THIS IS ORIGINALLY A PURE VIRTUAL METHOD AND NEEDS TO BE DEFINED */
    /* */
}

bool CGIRequestHandler::done() {
    return state_ == RESPONSE_READY;
}

void CGIRequestHandler::consume(DataView& view) {
    builder_.parse_headers(view);
    if (builder_.finished()) {
        state_ = WRITING_RESPONSE;
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
    // finished;
}

}
#include <signal.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include "Timestamp.hpp"
#include "CGIRequestHandler.hpp"
#include "Dispatcher.hpp"
#include "Request.hpp"
#include "CGIResolver.hpp"

namespace http {


CGIRequestHandler::CGIRequestHandler(  const ResolutionResult& res, http::Request const& req )
    : process_(cgi::CGIResolver::resolve(req, res)),
    stdin_(process_.stdin_pipe().write_end().get(), io::WRITABLE, *this),
    stdout_(process_.stdout_pipe().read_end().get(), io::READABLE, *this),
    stderr_(process_.stderr_pipe().read_end().get(), io::READABLE, *this) {

}

CGIRequestHandler::~CGIRequestHandler() {
    
}

void CGIRequestHandler::handle() {

}

bool CGIRequestHandler::done() {
    return true;
}

void CGIRequestHandler::consume( DataView& view ) {
    builder_.parse_headers(view);

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

}
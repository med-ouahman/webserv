#include <signal.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include "Timestamp.hpp"
#include "CGIRequestHandler.hpp"
#include "Dispatcher.hpp"

namespace http {

    const char* CGIRequestHandler::cgi_metadata[] = {"REQUEST_METHOD", "SERVER_PROTOCOL", "QUERY_STRING", NULL};

    const char* CGIRequestHandler::stripped_headers[] = {"transfer-encoding", "content-length", "content-type", "connection", NULL};

    CGIRequestHandler::CGIRequestHandler(  const ResolutionResult res_ )
        {}

    CGIRequestHandler::~CGIRequestHandler() {
        
    }

    void CGIRequestHandler::handle() {
    
    }

    bool CGIRequestHandler::done() {
        return true;
    }

}
#include "CGIHandler.hpp"
#include "EventLoop.hpp"
#include "Connection.hpp"

namespace http {

    CGIHandler::CGIHandler( const io::EventLoop& l, const core::Connection& con )
        :loop(l),
        conn(con),
        cgi_pid(-1),
        pipe_stdin(-1),
        pipe_stdout(-1),
        pipe_stderr(-1) {}

    CGIHandler::~CGIHandler() {}

    void CGIHandler::on_event( io::EventType event ) {
        if (event == io::ERROR) {
            
        }  
    }
}
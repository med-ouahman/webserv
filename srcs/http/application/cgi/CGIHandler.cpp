#include "CGIHandler.hpp"
#include "EventLoop.hpp"
#include "Connection.hpp"

namespace http {

    CGIHandler::CGIHandler( const io::EventLoop& l, const core::Connection& con )
        :loop(l),
        conn(con) {}

    CGIHandler::~CGIHandler() {}

    void CGIHandler::on_event( io::EventType event ) {
        if (event == io::ERROR) {
            
        }  
    }
}
#include "CGIHandler.hpp"

namespace http {

    CGIHandler::CGIHandler( const io::EventLoop& l, const core::Connection& con ): loop(l), conn(con) {

    }

    CGIHandler::~CGIHandler() {

    }

    void CGIHandler::on_event( io::EventType event ) {
        
    }
}
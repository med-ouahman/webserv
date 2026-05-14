#include "CGIHandler.hpp"
#include "Connection.hpp"

namespace http {

    ssize_t CGIHandler::produce_output( BufferWriter* writer ) {
        return conn.get_body_handler().produce_body_chunk(writer);
    }
    
}
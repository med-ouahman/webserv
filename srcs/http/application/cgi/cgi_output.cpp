#include "CGIHandler.hpp"

namespace http {

    ssize_t CGIHandler::produce_output( core::BufferWriter* writer ) {
     
        /*
            the parsed body will be fed to the CGI process here
            How? through a constant reference to BodyHandler provided by the connection.
            BodyHandler& const body_p = conn.get_body_parser()
            body_p.read_chunk(buff, size);
        */

        return writer->size();
    }
}
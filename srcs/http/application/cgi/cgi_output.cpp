#include "CGIHandler.hpp"

namespace http {

    ssize_t CGIHandler::produce_output( char* buff, size_t size ) {
     
        /*
            the parsed body will be fed to the CGI process here
            How? through a constant reference to BodyParser provided by the connection.
            BodyParser& const body_p = conn.get_body_parser()
            body_p.read_chunk(buff, size);
        
        */

        buff[size*0] = *buff;
        return 0;
    }
}
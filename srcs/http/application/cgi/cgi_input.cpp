#include "CGIHandler.hpp"
#include "Connection.hpp"
#include "HTTPResponse.hpp"

namespace http {

    void CGIHandler::on_input_ready( char* buff, size_t size ) {

        /*
            parisng and stuff done here.
            the CGI script will produce output in a form that needs to be scanned and parsed by ther server
            the CGI script outputs the status line in a different way than the RFC.
            headers may differ too
            what to do?
            parse the status line, headers from the CGI output, make them HTTP compliant, search for CGI output and how it works.
            add the headers to the response through response->add_header(key, val);
            for body, the CGIBodyProvider will be used, if no content-length header, use transfer-encoding: chunked
            
            use OutpuState to track progress across multiple calls
            use line_buff to store incomplete headers
            
            after detecting the body, you allocate the body provider and just pass it the buffer and size
        */

        HTTPResponse& response = conn.get_response();

        response.headers["key"] = "value";

    }

}

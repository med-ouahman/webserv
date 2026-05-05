#include "CGIHandler.hpp"
#include "Connection.hpp"
#include "HTTPResponse.hpp"
#include "CGIBodyProvider.hpp"

namespace http {

    ScanResult CGIHandler::on_input_ready( core::DataView* data_view ) {

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

            The LineScanner is a utility that helps detect CRLF line.
            LineScanner::scan(size), scans the given buffer for the pair \r\n, if found it is put in the lienebuff and access via LineScanner::line()
            the LineScanner::scan returns 3 values, SUCCESS a line was fully parsed, NEED_MORE: more data is needed or ERROR is case of error (size>max_size)
        */

        (void)data_view;
    
        if (output_state == CGIOutputState::HEADERS) {
            /*
                the headers are built and sent to the response object          
            */
        }

        if (output_state == CGIOutputState::BODY) {
            conn.on_cgi_output_ready(); // tells the connection to enable cgi body
            output_state = CGIOutputState::READING_BODY;
        }

        return SUCCESS;
    }

}

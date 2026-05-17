#include "CGIHandler.hpp"
#include "Connection.hpp"
#include "HTTPResponse.hpp"
#include "CGIBodyProvider.hpp"

namespace http {

    ScanResult CGIHandler::on_input_ready() {
       
        if (output_state == CGIOutputState::WRITING_BODY) {
            conn.on_cgi_output_ready();
            return SUCCESS;
        }
    
        if (output_state == CGIOutputState::HEADERS) {
            
        }

        if (output_state == CGIOutputState::BODY) {
            conn.bind_cgi();
            output_state = CGIOutputState::WRITING_BODY;
        }

        return SUCCESS;
    }

}

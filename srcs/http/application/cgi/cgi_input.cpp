#include "CGIHandler.hpp"
#include "Connection.hpp"
#include "HTTPResponse.hpp"
#include "CGIBodyProvider.hpp"

namespace http {

    ScanResult CGIHandler::on_input_ready() {
       
        std::cout << "B: " << stdout_ch_view.size() <<"\n";
        if (output_state == CGIOutputState::WRITING_BODY)
            return SUCCESS;
    
        if (output_state == CGIOutputState::HEADERS
            || output_state == CGIOutputState::STATUS_LINE) {
            std::cout << "Headers\n";
            output_state = CGIOutputState::BODY;
            /* PARSE HEADERS HERE PLEASE */
            
        }

        if (output_state == CGIOutputState::BODY) {
            conn.on_cgi_output_ready();
            output_state = CGIOutputState::WRITING_BODY;
        }

        return SUCCESS;
    }

}

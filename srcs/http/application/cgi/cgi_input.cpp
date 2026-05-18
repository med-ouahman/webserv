#include "CGIHandler.hpp"
#include "Connection.hpp"
#include "HTTPResponse.hpp"
#include "CGIBodyProvider.hpp"
#include "Parser.hpp"

namespace http {


    ScanResult CGIHandler::on_input_ready() {
       
        if (parse_ctx.state == CGIOutputState::WRITING_BODY) {
            conn.on_cgi_output_ready();
            return SUCCESS;
        }
    
        if (parse_ctx.state == CGIOutputState::HEADERS) {
            parse_cgi_headers();
        }

        if (parse_ctx.state == CGIOutputState::BODY) {
            conn.bind_cgi();
            parse_ctx.state = CGIOutputState::WRITING_BODY;
        }

        return SUCCESS;
    }

}

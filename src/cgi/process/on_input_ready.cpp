#include "CGIRequestHandler.hpp"
#include "Connection.hpp"
#include "Response.hpp"
#include "CGIBodyProvider.hpp"
#include "Parser.hpp"

namespace http {


    ScanResult CGIRequestHandler::on_input_ready() {
       
        if (parse_ctx.state == CGIOutputState::WRITING_BODY) {
            conn.on_cgi_output_ready();
            return SUCCESS;
        }
    
        if (parse_ctx.state == CGIOutputState::HEADERS
                || parse_ctx.state == CGIOutputState::STATUS_LINE) {
            std::cout << "CGI HEADERS\n";
            parse_cgi_headers();
        }

        if (parse_ctx.state == CGIOutputState::BIND_BODY) {
            conn.bind_cgi();
            parse_ctx.state = CGIOutputState::WRITING_BODY;
        }

        return SUCCESS;
    }

}

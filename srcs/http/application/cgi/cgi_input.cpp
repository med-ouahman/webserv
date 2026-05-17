#include "CGIHandler.hpp"
#include "Connection.hpp"
#include "HTTPResponse.hpp"
#include "CGIBodyProvider.hpp"
#include "Parser.hpp"

namespace http {

    void CGIHandler::parse_cgi_headers() {

        while (true) {
            scanner.scan(MAX_CGI_HEADER_LEN);

            Base::Expected<std::pair<std::string, std::string>, int> header = Parser::parse_header(scanner.line());
            
            if (!header.has_value()) {
                cgi_state = CGIState::ERROR;
                conn.on_cgi_error(BAD_GATEWAY);
                return ;
            }
            
        }

        
        
    }

    ScanResult CGIHandler::on_input_ready() {
       
        if (output_state == CGIOutputState::WRITING_BODY) {
            conn.on_cgi_output_ready();
            return SUCCESS;
        }
    
        if (output_state == CGIOutputState::HEADERS) {
            parse_cgi_headers();
        }

        if (output_state == CGIOutputState::BODY) {
            conn.bind_cgi();
            output_state = CGIOutputState::WRITING_BODY;
        }

        return SUCCESS;
    }

}

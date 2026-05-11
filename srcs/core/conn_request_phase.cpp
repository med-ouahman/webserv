#include "Connection.hpp"

namespace core {

    void Connection::on_client_error() {
       http::HTTPDispatcher::build_error_response(http::BAD_REQUEST, "Bad request");
        state = ConnectionState::WRITING;
        close_after_write = true;
    }

    void Connection::request_building() {

        http::ScanResult result = p.parse();

        switch (result) {
            case http::NEED_MORE:
                processing = false;
                break;
            case http::ERROR:
                processing = false;
                on_client_error();
                break;
            case http::SUCCESS:
                phase = RequestPhase::RESOLVING;
                break;
        }

    }

    void Connection::request_resloving() {
        
        current_res = http::HTTPDispatcher::resolve(p.get_request());

        if (current_res.body_policy.type == http::BodyType::ERROR)
        {
            on_client_error();
            processing = false;
        } 
        else if (current_res.body_policy.type != http::BodyType::NONE)
        {
            body_handler.prepare_body(current_res.body_policy);
            phase = RequestPhase::READING_BODY;
        }
        else
        {
            phase = RequestPhase::PROCESSING;
        }
    }

    void Connection::request_processing() {
        processing = false;

        if (current_res.type == http::HTTPResponseType::CGI)
            invoke_cgi(http::HTTPDispatcher::get_cgi_context(p.get_request(), current_res));
        else
           http::HTTPDispatcher::handle_request(current_res, p.get_request());
        phase = RequestPhase::FINAL;
    }

    void Connection::request_reading_body() {
        http::ScanResult r = body_handler.read_body();
        
        switch (r) {
            case http::SUCCESS:
                phase = RequestPhase::PROCESSING;
                break;
            case http::ERROR:
                phase = RequestPhase::ERROR;
                processing = false;
                break;
            case http::NEED_MORE:
                processing = false;
                break;
        }
    }

}

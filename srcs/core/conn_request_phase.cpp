#include "Connection.hpp"

namespace core {


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
        
        current_res = dispatcher.resolve(p.get_request(), config.server);

        if (current_res.body_type == http::BodyType::ERROR)
        {
            on_client_error();
            processing = false;
        } 
        else if (current_res.body_type != http::BodyType::NONE)
        {
            body_handler.prepare_body(current_res.body_type, current_res.body_storage, current_res.path, current_res.parsed_body_size);
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
            invoke_cgi(dispatcher.get_cgi_context(p.get_request(), current_res));
        else
            dispatcher.handle_request(current_res, p.get_request());
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
                break;   
        }
    }

}

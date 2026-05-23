#include "Connection.hpp"

namespace core {

    void Connection::on_client_error() {
        response.build_error_response(http::BAD_REQUEST);
        state = ConnectionState::WRITING;
        close_after_write = true;
    }

    void Connection::request_building() {

        http::ScanResult result = http::parser::parse_http_request(line_scanner, request);
        
        switch (result) {
            case http::NEED_MORE:
                processing = false;
                break;
            case http::ERROR:
                phase = RequestPhase::ERROR;
                break;
            case http::SUCCESS:
                last_.update();
                phase = request.finished() ? RequestPhase::RESOLVING: RequestPhase::BUILDING;
                processing = phase == RequestPhase::RESOLVING;
                break;
        }

    }

    void Connection::request_resloving() {

        http::ResolutionResult result = http::Dispatcher::resolve(request);

        request_handler = http::Dispatcher::create_request_handler(*this, result);
        
        http::BodyConf b = http::Dispatcher::configure_body(request, result);
        
        if (b.type == http::BodyType::ERROR) {
            on_client_error();
            processing = false;
            return ;
        }
        
        if (b.type == http::BodyType::NONE) {
            phase = RequestPhase::PROCESSING;
            return ;
        }
        std::cout << b.type << "\n";
        body_handler.prepare_body(b);
        
        phase = RequestPhase::READING_BODY;
    }

    void Connection::request_processing() {
        request_handler->handle();
        processing = false;
        phase = request_handler->done()
            ? RequestPhase::FINAL
            : phase;
    }

    void Connection::request_reading_body() {
        
        http::ScanResult r = body_handler.read_body();

        body_bytes_received += body_handler.parsed_bytes();
        
        switch (r) {
            case http::SUCCESS:
                last_.update();
                body_bytes_received = 0;
                std::cout << "Body Read Successfully\n";
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

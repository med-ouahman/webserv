#include "Connection.hpp"

namespace core {

    void Connection::on_client_error() {
        response.build_error_response(http::BAD_REQUEST);
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
                phase = RequestPhase::ERROR;
                break;
            case http::SUCCESS:
                last_.update();
                phase = p.finished() ? RequestPhase::RESOLVING: RequestPhase::BUILDING;
                processing = phase == RequestPhase::RESOLVING;
                break;
        }

    }

    void Connection::request_resloving() {
        
        response.status_code = http::OK;

        response.headers["content-type"] = "text/html";
        response.headers["content-length"] = "300";
        response.headers["transfer-encoding"] = "chunked";
        response.headers["User-Agent"] = "Firefox";

        phase = RequestPhase::WRITING_RESPONSE;
        return ;

        http::ResolutionResult result = http::HTTPDispatcher::resolve(p.get_request_data());

        request_handler = http::HTTPDispatcher::create_request_handler(*this, result);
        
        http::BodyConf b = http::HTTPDispatcher::configure_body(p.get_request_data(), result);
        
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

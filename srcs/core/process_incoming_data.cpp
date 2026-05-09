
#include "Connection.hpp"
#include "LineScanner.hpp"

namespace core {
  
    void Connection::process_incoming_data() {

        http::ScanResult result;
        
        result = p.parse();
        
        if (p.finished())
        {
            body_handler.detect_body_type(p.get_request().headers);
            body_handler.read_body("");
        }

        switch (result) {
            case http::NEED_MORE:
                processing = true;
                break;
            case http::ERROR:
               on_client_error();
               processing = false;
               break;
            case http::SUCCESS:
                on_request_ready();
                processing = false;
            default:
                break;
        }
    }

}

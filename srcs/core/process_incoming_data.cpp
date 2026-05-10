
#include "Connection.hpp"
#include "LineScanner.hpp"

namespace core {
  
    void Connection::process_incoming_data() {

        http::ScanResult result;
        
        result = p.parse();
        
        switch (result) {
            case http::NEED_MORE:
                break;
            case http::ERROR:
               on_client_error();
               break;
            case http::SUCCESS:
                on_request_ready();
            default:
                break;
        }
    }

}

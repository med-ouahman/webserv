
#include "Connection.hpp"
#include "LineScanner.hpp"

namespace core {
  
    void Connection::process_incoming_data() {

        while (true) {

            switch (phase) {   
                case RequestPhase::BUILDING:
                    p.parse();
                    break;
                case RequestPhase::RESOLVING:
                    dispatcher.resolve(p.get_request(), config.server);
                    break;
                case RequestPhase::PROCESSING:
                /* */
            }
        }

        // switch (result) {
        //     case http::NEED_MORE:
        //         break;
        //     case http::ERROR:
        //        on_client_error();
        //        break;
        //     case http::SUCCESS:
        //         on_request_ready();
        //     default:
        //         break;
        // }

    }

}

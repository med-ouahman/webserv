
#include "Connection.hpp"
#include "LineScanner.hpp"

namespace core {
  
    void Connection::process_incoming_data() {

        processing = true;
        while (processing) {
            switch (phase) {
                case RequestPhase::BUILDING:
                    request_building();
                    break;
                case RequestPhase::RESOLVING:
                    request_resloving();
                    break;
                case RequestPhase::READING_BODY:
                    request_reading_body();
                    break;
                case RequestPhase::PROCESSING:
                    request_processing();
                    break;
                case RequestPhase::ERROR:
                    processing = false;
                    on_client_error();
                    break;
                case RequestPhase::FINAL:
                    processing = false;
                    break;
            }
        }
    }
}

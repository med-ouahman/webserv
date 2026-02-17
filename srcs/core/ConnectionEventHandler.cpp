
#include "Connection.hpp"
#include "ConnectionStateMachine.hpp"

namespace core {

    void Connection::on_event( io::EventType event ) {
        
        switch (event) {
            case io::WRITABLE: {
                state = WRITING;
                break;
            }
            case io::READABLE: {
                state = READING;
                break;
            }
            case io::ERROR: {
                state = ERROR;
                break;
            }
        }
    }
}

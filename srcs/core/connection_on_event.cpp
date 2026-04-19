
#include "Connection.hpp"

namespace core {

    void Connection::on_event( io::EventType event ) {
        
        switch (event) {
            case io::WRITABLE: {
                state = ConnectionState::WRITING;
                break;
            }
            case io::READABLE: {
                state = ConnectionState::READING;
                break;
            }
            case io::ERROR: {
                state = ConnectionState::CLOSING;
                break;
            }
        }
    }
}

#include "Connection.hpp"

namespace core {

    void Connection::error( void ) {
        state = ConnectionState::CLOSING;
    }
}

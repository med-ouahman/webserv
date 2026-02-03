#pragma once

#include "ConnectionState.hpp"
#include "ConnectionEvent.hpp"

namespace core {
    class ConnectionStateMachine {
        public:
            static ConnectionState next_state(ConnectionState currentState, ConnectionEvent event);            
    };
}

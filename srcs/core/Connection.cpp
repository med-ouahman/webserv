#include "Connection.hpp"
#include "ConnectionStateMachine.hpp"
#include <cassert>
#define NDEBUG 0
namespace core {
    Connection::Connection( int fd ): fd(fd) {}

    Connection::~Connection() {
        if (fd > 0) {
            close(fd);
        }
    }
    void Connection::handle_event(ConnectionEvent event) {
        ConnectionState nextState = ConnectionStateMachine::next_state(state, event);
        #ifdef NDEBUG
        if (nextState == ERROR && state != ERROR) {
            assert(false && "Illegal transition");    
        }
        #endif
        state = nextState;
    }
}

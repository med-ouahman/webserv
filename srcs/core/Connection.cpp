
#include "Connection.hpp"
#include "ConnectionStateMachine.hpp"
#include "Connection.hpp"

#define NDEBUG 0
#include <cassert>

namespace core {
    
    Connection::Connection( int fd ): fd(fd), state(ACCEPTED) {}

    Connection::~Connection() {
        if (fd > 0) {
            close(fd);
        }
        state = CLOSING;
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

    ConnectionAction Connection::desired_action() const {

        ConnectionAction action = { false, false, false };
        
        switch (state) {
            case READING:
                action.want_read = true;
                break;
            case WRITING:
                action.want_write = true;
                break;
            case CLOSING:
                action.want_close = true;
            default:
                break;
        }
        
        return action;
    }

    int Connection::get_fd() const {
        return fd;
    }
}
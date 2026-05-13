
#include "ConnectionStateMachine.hpp"
#include "Connection.hpp"
#include <unistd.h>
#include <cstring>
#include <cerrno>

namespace core {
    
    Connection::Connection( int _fd, uint32_t mask, io::EventLoop& l )
        : Stream(_fd),
        
        event_mask(mask),
        state(ConnectionState::IDLE),
        phase(RequestPhase::BUILDING),
        p(data_view),
        body_handler(_fd, data_view),
        close_after_write(false),
        num_requests(0),
        loop(l) {}

    Connection::~Connection() {
        state = ConnectionState::CLOSING;
    }

    ConnectionAction Connection::desired_action() const {
        ConnectionAction action;

        switch (state) {
            case ConnectionState::READING:
                action.want_read = true;
                break;
            case ConnectionState::WRITING:
                action.want_write = true;
                break;
            case ConnectionState::CLOSING:
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

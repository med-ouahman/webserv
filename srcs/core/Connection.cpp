
#include "ConnectionStateMachine.hpp"
#include "Connection.hpp"
#include <unistd.h>
#include <cstring>
#include <cerrno>

namespace core {
    
    Connection::Connection( int _fd, const config::Config& conf, uint32_t mask, io::EventLoop& l )
        : Stream(_fd),
        
        event_mask(mask),
        loop(l),
        state(ConnectionState::IDLE),
        phase(RequestPhase::BUILDING),
        p(conf, data_view),
        body_handler(_fd, data_view),
        dispatcher(conf),
        config(conf),
        close_after_write(false),
        num_requests(0),
        cgi_handler(NULL) {}

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

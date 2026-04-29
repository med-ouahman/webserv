
#include "ConnectionStateMachine.hpp"
#include "Connection.hpp"
#include <unistd.h>
#include <cstring>
#include <cerrno>

namespace core {
    
    Connection::Connection( int _fd, const config::Config& conf, uint32_t mask, const io::EventLoop& l )
        :Stream(_fd),
        processing(true),
        event_mask(mask),
        loop(l),
        state(ConnectionState::IDLE),
        p(_fd, conf),
        dispatcher(conf),
        config(conf),
        close_after_write(false),
        num_requests(0),
        ms_(0),
        cgi_handler(NULL) {}

    Connection::~Connection() {
    
        state = ConnectionState::CLOSING;
    }

    ConnectionAction Connection::desired_action( void ) const {
        
        ConnectionAction action = { false, false, false, false };
    
        switch (state) {
            case ConnectionState::READING:
                action.want_read = true;
                break;
            case ConnectionState::WRITING:
                action.want_write = true;
                break;
            case ConnectionState::CLOSING:
                action.want_close = true;
                break;
            default:
                break;
        }

        return action;
    }

    int Connection::get_fd( void ) const {
        return fd;
    }

    


    void Connection::tick( void ) {
        
        if (ConnectionState::IDLE == state) {
            ms_ += 0;
        } else {
            ms_ = 0;
        }

        if (ms_ > MAX_IDLE_TIMEOUT) {
            state = ConnectionState::CLOSING;
        }
    }
}

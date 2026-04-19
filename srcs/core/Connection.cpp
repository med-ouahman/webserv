
#include "ConnectionStateMachine.hpp"
#include "Connection.hpp"
#include <unistd.h>
#include <cstring>
#include <cerrno>

namespace core {
    
    Connection::Connection( int _fd, const config::Config& conf, uint32_t mask, const io::EventLoop& loop )
        :fd(_fd),
        event_mask(mask),
        state(ConnectionState::IDLE),
        p(_fd, conf),
        config(conf),
        close_after_write(false),
        num_requests(0),
        bytes_in_buff(0),
        sent_offset(0),
        bytes_received(0),
        inactivity_ticks(0),
        cgi_handler(loop, *this) {}

    Connection::~Connection() {
        if (fd >= 0) {
            ::close(fd);
        }
        state = ConnectionState::CLOSING;
    }

    ConnectionAction Connection::desired_action( void ) const {
        
        ConnectionAction action = { false, false, false, false };
        
        if (bytes_received > 0) {
            action.want_process = true;
        }

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

    bool Connection::on_read( ::ssize_t bytes ) {
   
        if (bytes <= 0) {
            if (bytes == 0) {
                state = ConnectionState::CLOSING;
            }
            return false;
        }
        inactivity_ticks = 0;
        bytes_received = bytes;
        return true;
    }

    bool Connection::read_buff_empty() const {
        return bytes_received == 0;
    }

    void Connection::tick( void ) {
        ++inactivity_ticks;
        if (inactivity_ticks > MAX_INACTIVITY_LIMIT) {
            state = ConnectionState::CLOSING;
        }
    }
}

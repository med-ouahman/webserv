
#include "ConnectionStateMachine.hpp"
#include "Connection.hpp"
#include <unistd.h>
#include <cstring>
#include <cerrno>

namespace core {
    
    Connection::Connection( int _fd, const config::ServerConfig* conf, uint32_t mask, const io::EventLoop& loop )
        :fd(_fd),
        event_mask(mask),
        state(ACCEPTED),
        p(_fd),
        server_conf(conf),
        close_after_write(false),
        num_requests(0),
        bytes_in_buff(0),
        sent_offset(0),
        bytes_received(0),
        read_buff_drained(true),
        inactivity_ticks(0),
        cgi_handler(loop, *this),
        progress(false) {}

    Connection::~Connection() {
        if (fd >= 0) {
            ::close(fd);
        }
        state = CLOSING;
    }

    ConnectionAction Connection::desired_action( void ) const {
        
        ConnectionAction action = { false, false, false, false };
        if (!read_buff_drained) {
            action.want_process = true;
        }

        switch (state) {
            case READING:
                action.want_read = true;
                break;
            case WRITING:
                action.want_write = true;
                break;
            case CLOSING:
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

    bool Connection::set_readbuff( ::ssize_t bytes ) {
        if (bytes <= 0) {
            if (bytes == 0) {
                state = CLOSING;
            }
            ++inactivity_ticks;
            return false;
        }
        inactivity_ticks = 0;
        bytes_received = bytes;
        read_buff_drained = false;
        return true;
    }

    bool Connection::read_buff_empty() const {
        return read_buff_drained;
    }
}

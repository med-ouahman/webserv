
#include "ConnectionStateMachine.hpp"
#include "Connection.hpp"
#include <unistd.h>
#include <cstring>
#include <cerrno>

namespace core {
    
    Connection::Connection( int fd, const config::ServerConfig* conf, uint32_t mask )
        :fd(fd),
        event_mask(mask),
        state(ACCEPTED),
        p(fd),
        server_conf(conf),
        close_after_write(false),
        num_requests(0),
        bytes_in_buff(0),
        sent_offset(0) {}

    Connection::~Connection() {
        if (fd >= 0) {
            ::close(fd);
        }
        state = CLOSING;
    }

    ConnectionAction Connection::desired_action( void ) const {

        ConnectionAction action = { false, false, false, false };
        
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

}


#include "ConnectionStateMachine.hpp"
#include "Connection.hpp"
#include <unistd.h>
#define NDEBUG 0
#include <cassert>
#include <cstring>

namespace core {
    
    Connection::Connection( int fd ): fd(fd), state(ACCEPTED) {}

    Connection::~Connection() {
        if (fd >= 0) {
            close(fd);
        }
        state = CLOSING;
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

    bool Connection::on_bytes( char* buff ) {
        
        while (http::HTTPParser::NEED_MORE_BYTES == p.consume(buff)) {
            
        }
        return false;
    }
    
    size_t Connection::peek_bytes( char* buff, size_t size ) {
        
        strncpy(buff, s.c_str(), size);
        return size;
    }

    void Connection::consume_bytes( size_t bytes ) {
        s = s.erase(0, bytes);
    }

    void Connection::on_close( void ) {
        if (fd >= 0) {
            close(fd);
            fd = -1;
        }
        state = CLOSING;
    }
}

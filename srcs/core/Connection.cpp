#include "Connection.hpp"
#include <unistd.h>
#include <cstring>
#include <cerrno>

namespace core {
    
    Connection::Connection( int _fd, uint32_t mask, io::EventLoop& l )
        : Stream(_fd),
        
        event_mask(mask),
        state(ConnectionState::IDLE),
        phase(RequestPhase::INITIAL),
        p(data_view),
        body_handler(_fd, data_view),
        close_after_write(false),
        num_requests(0),
        body_bytes_received(0),
        loop(l) {}

    Connection::~Connection() {
        state = ConnectionState::CLOSING;
    }

    ConnectionAction Connection::action() const {
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

    bool Connection::timedout() {
        
        double limit = Limits::MAX_IDLE_TIMEOUT;
       
        switch (phase) {
            case core::RequestPhase::INITIAL:
                limit = Limits::MAX_INITIAL_TIMEOUT;
                break;
            case core::RequestPhase::BUILDING:
                limit = Limits::MAX_HEADER_TIMEOUT;
                break;
            case core::RequestPhase::READING_BODY:
                limit = Limits::MAX_BODY_PROGRESS_TIMEOUT;
                break;
            case core::RequestPhase::IDLE:
                limit = Limits::MAX_IDLE_TIMEOUT;
                break;
            case core::RequestPhase::PROCESSING: /* OR BETTER CGI*/
                return false;
            default:
                limit = Limits::MAX_IDLE_TIMEOUT;
        }

        if (last_.elapsed() >= limit) {
            return true;
        }

        return false;
    }
}

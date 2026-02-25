
#include "ConnectionStateMachine.hpp"
#include "Connection.hpp"
#include <unistd.h>
#define NDEBUG 0
#include <cassert>
#include <cstring>

namespace core {
    
    Connection::Connection( int fd, const config::ServerConfig* conf ): fd(fd), state(ACCEPTED), server_conf(conf) {}

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
        
        while (true) {
            http::HTTPParser::ParseResult result = p.consume(buff);
            if (result == http::HTTPParser::NEED_MORE_BYTES) {
                break;
            } else if (result == http::HTTPParser::PARSE_ERROR) {
                return false;
            }
            http::HTTPRequest req = p.get_request();
            p.reset();
            handler.handle_request(req);
            std::string response_buff = handler.serialize();
            queue_response(response_buff);
        }
        return true;
    }

    void Connection::on_close( void ) {
        if (fd >= 0) {
            close(fd);
            fd = -1;
        }
        state = CLOSING;
    }

    void Connection::queue_response( std::string const& response_buff ) {
        (void)response_buff;
    }
}


#include "ConnectionStateMachine.hpp"
#include "Connection.hpp"
#include <unistd.h>
#include <cstring>

namespace core {
    
    Connection::Connection( int fd, const config::ServerConfig* conf, uint32_t mask ): fd(fd), event_mask(mask), state(ACCEPTED), server_conf(conf) {}

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
                break;
            case WRITE_CLOSE: {
                action.want_write = true;
                action.want_close = true;
                break;
            }
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
                std::cout << "Bad Request\n";
                handler.build_error_response(http::BAD_REQUEST, "Bad request");
                state = WRITE_CLOSE;
                return false;
            }

            http::HTTPRequest req = p.get_request();
            p.reset();
            handler.handle_request(req);
            response_buff = handler.serialize();
            queue_response();
        }

        return true;
    }

    void Connection::queue_response( void ) {
        // (void)response_buff;
    }

    void Connection::update_buff( size_t sent_bytes ) {
        sent_offset += sent_bytes;
        if (sent_offset == response_buff.size()) {
            response_buff.clear();
            sent_offset = 0;
        }
    }

    const std::string& Connection::get_write_buff( void ) const {
        return response_buff;
    }

    size_t Connection::get_sent_offset( void ) const {
        return sent_offset;
    }
}

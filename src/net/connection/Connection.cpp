#include "Connection.hpp"
#include <unistd.h>
#include <cstring>
#include <cerrno>

namespace net {
    
    Connection::Connection( int _fd, io::EventMask mask )
        : stream(_fd, mask),
        state(READING) {}

    Connection::~Connection() {}

    ConnectionAction Connection::action() const {

        switch (state) {
            case ConnectionState::READING:
                return READ;
            case ConnectionState::WRITING:
                return WRITE;
            case ConnectionState::CLOSING:
                return CLOSE;
            default:
                break;
        }

        return CLOSE;
    }

    bool Connection::timedout() {
        return false;
    }

    void Connection::consume( DataView& view ) {
        session.consume(view);
    }

    void Connection::produce( BufferWriter& writer ) {
        session.produce(writer);
    }

    void Connection::on_stream_error() {
        state = CLOSING;
    }

    void Connection::on_stream_closed() {
        state = CLOSING;
    }
    
}

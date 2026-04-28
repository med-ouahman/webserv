#include "Connection.hpp"

namespace core {

    void Connection::process( void ) {

        if (event_type == io::READABLE) {
            state = ConnectionState::READING;
        } else if (event_type == io::WRITABLE) {
            state = ConnectionState::WRITING;
        }

        switch (state) {
            case ConnectionState::READING:
                on_read();
                break;
            case ConnectionState::WRITING:
                on_write();
                break;
        }

    }
    
}
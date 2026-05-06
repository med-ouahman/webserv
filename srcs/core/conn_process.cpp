#include "Connection.hpp"

namespace core {

    void Connection::process() {

        processing = state != ConnectionState::CLOSING;
        
        while (processing) {

            if (state == ConnectionState::READING) {
                on_readable();
            }
            
            if (state == ConnectionState::WRITING) {
                on_writeable();
            }
        }
        
    }

    void Connection::on_read_eof() {
        state = ConnectionState::CLOSING;
        processing = false;
    }

    void Connection::on_read_error() {

    }
}

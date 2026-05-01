#include "Connection.hpp"

namespace core {

    bool Connection::process() {

        processing = state != ConnectionState::CLOSING;
        
        while (processing) {

            if (state == ConnectionState::READING) {
                on_readable();
            }
            
            if (state == ConnectionState::WRITING) {
                on_writeable();
            }
        }
        
        return processing;
    }
}

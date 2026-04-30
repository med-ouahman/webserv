#include "Connection.hpp"

namespace core {

    bool Connection::process( void ) {

        processing = true;
        
        while (processing) {

            if (state == ConnectionState::READING) {
                on_readable();
            } else if (state == ConnectionState::WRITING) {
                on_writeable();
            }
        }
        
        return processing;
    }
}

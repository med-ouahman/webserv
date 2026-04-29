#include "Connection.hpp"

namespace core {

    bool Connection::process( void ) {

        while (processing) {

            if (state == ConnectionState::READING) {
                on_readable();
                process_incoming_data();
            }
            
            if (state == ConnectionState::WRITING) {
                on_writeable();
                process_outgoing_data();
            }
        }
        
        return processing;
    }
}

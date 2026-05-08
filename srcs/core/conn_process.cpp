#include "Connection.hpp"

namespace core {

    void Connection::process() {

        if (resume_task) std::cout << "TASK RESUME\n";
        resume_task = false;
        
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
        processing = false;
    }
}

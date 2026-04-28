#include "Stream.hpp"

namespace io {

    void Stream::on_event( EventType event ) {
        
        switch (event) {
            case EventType::WRITABLE:
                io_state = IOState::WRITING;
                write();
                break;
            case EventType::READABLE:
                io_state = IOState::READING;
                read();
                break;
            case EventType::ERROR:
                io_state = IOState::ERROR;
                error(); // pure virtual, the derived class has its own error handling mechanism
                return ;
            default:
                break;
        }
    } 
}
#pragma once

#include "ConnectionState.hpp"
#include "ConnectionEvent.hpp"
#include "ConnectionAction.hpp"
#include "IOHandler.hpp"
#include <unistd.h>

namespace core {
    class Connection: public io::IOHandler {
        private:
            int fd;
            ConnectionState state;
        public:
            explicit Connection( int fd );
            ~Connection();
            int get_fd() const;
            void handle_event( ConnectionEvent event );
            ConnectionAction desired_action() const;
            // void on_event( ConnectionEvent event );
            void on_event( uint32_t events );
    };   
}

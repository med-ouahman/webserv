#pragma once

#include "ConnectionState.hpp"
#include "ConnectionEvent.hpp"
#include <unistd.h>

namespace core {
    class Connection {
        private:
            int fd;
            ConnectionState state;
        public:
            explicit Connection( int fd );
            ~Connection();
            void handle_event( ConnectionEvent event );
    };   
}

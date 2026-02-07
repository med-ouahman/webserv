#pragma once

#include "ConnectionState.hpp"
#include "ConnectionEvent.hpp"
#include "ConnectionAction.hpp"
#include <unistd.h>

namespace core {
    class Connection {
        private:
            int fd;
            ConnectionState state;
        public:
            explicit Connection( int fd );
            ~Connection();
            int get_fd() const;
            void handle_event( ConnectionEvent event );
            ConnectionAction desired_action() const;
            void on_event( ConnectionEvent event );
    };   
}

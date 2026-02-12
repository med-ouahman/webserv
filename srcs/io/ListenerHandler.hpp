#pragma once

#include "IOHandler.hpp"

namespace io {

    class ListenerHandler: public IOHandler {
        public:
            void on_event( uint32_t event );
        private:
            void on_readable();
            void on_error();
    };
}

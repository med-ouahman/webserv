#pragma once

#include "AIOHandler.hpp"

namespace io {

    class ListenerHandler: public AIOHandler {
        public:
            void on_event( uint32_t event );
        private:
            void on_readable();
            void on_error();
    };
}

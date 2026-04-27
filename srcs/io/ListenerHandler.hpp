#pragma once

#include "IIOHandler.hpp"

namespace io {

    class ListenerHandler: public IIOHandler {
        public:
            void on_event( uint32_t event );
        private:
            void on_readable();
            void on_error();
    };
}

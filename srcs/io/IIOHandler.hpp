
#pragma once

#include <stdint.h>
#include "EventType.hpp"

namespace io {

    class IIOHandler {
        protected:
            int fd;

        public:
            virtual void on_event( EventType event ) = 0;
            virtual ~IIOHandler() {};
    };
}

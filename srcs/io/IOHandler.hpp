
#pragma once

#include <stdint.h>
#include "EventType.hpp"

namespace io {

    class IOHandler {
        public:
            virtual void on_event( EventType event ) = 0;
            virtual ~IOHandler() {};
    };
}


#pragma once

#include <stdint.h>

namespace io {

    class IOHandler {
        public:
        virtual void on_event( uint32_t event ) = 0;
        virtual ~IOHandler() {};
    };
}

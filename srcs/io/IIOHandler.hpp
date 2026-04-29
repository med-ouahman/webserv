
#pragma once

#include <stdint.h>
#include "EventType.hpp"
#include <unistd.h>

namespace io {

    class IIOHandler {
        protected:
            int fd;

        public:
            IIOHandler( int fd_ ): fd(fd_) {};
            virtual void on_event( EventType event ) = 0;
            virtual ~IIOHandler() {
                if (fd >= 0) {
                    ::close(fd);
                    fd = -1;
                }
            };
    };
}

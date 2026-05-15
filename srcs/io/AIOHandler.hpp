
#pragma once

#include <stdint.h>
#include "EventType.hpp"
#include <unistd.h>

namespace io {

    class AIOHandler {
        protected:
            int fd_;

        public:
            AIOHandler( int fd__ ): fd_(fd__) {};
            virtual void on_event( EventType event ) = 0;
            virtual ~AIOHandler() {
                if (fd_ >= 0) {
                    ::close(fd_);
                    fd_ = -1;
                }
            };

            int fd() const { return fd_; }
    };
}

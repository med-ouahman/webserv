
#pragma once

#include <stdint.h>
#include "EventType.hpp"
#include <unistd.h>

namespace io {

    typedef uint32_t EventMask;

    class AEventHandler {
        protected:
            int fd_;
            EventMask mask_;
        public:
            AEventHandler( int fd__ ): fd_(fd__) {};
            virtual void on_event( EventType event ) = 0;
            virtual ~AEventHandler() {
                if (fd_ >= 0) {
                    ::close(fd_);
                    fd_ = -1;
                }
            };

            int fd() const { return fd_; }
            EventMask mask() const { return mask_; }
            void update_mask( EventMask m ) { mask_ = m; }
    };
}

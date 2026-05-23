
#pragma once

#include <stdint.h>
#include "EventType.hpp"
#include <unistd.h>

namespace io {

    typedef uint32_t EventMask;

    class AEventHandler {
        private:
            int fd_;
            EventMask mask_;
            AEventHandler( const AEventHandler& );
            AEventHandler& operator=( const AEventHandler& );
            
        public:
            AEventHandler( int fd__, EventMask msk ): fd_(fd__), mask_(msk) {};
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

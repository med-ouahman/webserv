
#pragma once

#include <stdint.h>

#include <unistd.h>

namespace io {

enum Event {
    NONE     = 0,
    WRITABLE = 1 << 0,
    READABLE = 1 << 1,
    HUP      = 1 << 2,
    RHUP     = 1 << 3,
    ERROR    = 1 << 4
};

class AEventHandler {
private:
    int fd_;
    Event mask_;
    Event applied_mask_;

    AEventHandler( const AEventHandler& );
    AEventHandler& operator=( const AEventHandler& );
    
public:
    AEventHandler( int fd__, Event msk ): fd_(fd__), mask_(msk), applied_mask_(msk) {};
    virtual void on_event( Event event ) = 0;
    virtual ~AEventHandler() {
        if (fd_ >= 0) {
            ::close(fd_);
            fd_ = -1;
        }
    };

    int fd() const { return fd_; }
    Event mask() const { return mask_; }
    void update_mask( Event m ) { mask_ = m; }
    bool is_synced() const {
        return applied_mask_ == mask_; 
    }

    void sync() {
        applied_mask_ = mask_;
    }
};
}




#pragma once

#include <unistd.h>

namespace io {

enum Event {
    NONE     = 0,
    WRITABLE = 1 << 0,
    READABLE = 1 << 1,
    HUP      = 1 << 2,
    RHUP     = 1 << 3,
    CLOSE    = 1 << 4,
    ERROR    = 1 << 5
};

class AEventHandler {
private:
    int fd_;
    Event events_;
    Event applied_;
    AEventHandler(const AEventHandler&);
    AEventHandler& operator=(const AEventHandler&);
    
public:
    AEventHandler(int __fd, Event events): fd_(__fd), events_(events), applied_(events) {};
    virtual void on_event(Event event) = 0;
    virtual ~AEventHandler() {
        if (fd_ >= 0) {
            ::close(fd_);
            fd_ = -1;
        }
    };

    int fd() const { return fd_; }
    Event events() const { return events_; }
    void update_events(Event new_ev) { events_ = new_ev; }
    void sync_events() { applied_ = events_; }
    bool synced() const { return applied_ == events_; }
};

}



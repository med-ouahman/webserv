
#pragma once

#include <unistd.h>

namespace io
{

enum Event
{

None     = 0,
Writable = 1 << 0,
Readable = 1 << 1,
Hup      = 1 << 2,
RHup     = 1 << 3,
Error    = 1 << 4,
Close    = 1 << 5
};

class AEventHandler
{

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

void close() {
    events_ = None;
    applied_ = None;
    ::close(fd_);
    fd_ = -1;
}

};

}


#pragma once

namespace io {
    enum EventType {
        NONE,
        WRITABLE,
        READABLE,
        HUP,
        RHUP,
        ERROR
    };
}


#include "Stream.hpp"

namespace io {

    void Stream::on_event( EventType event ) {
        io_event = event;
        handle_event();
        process();
    }
}

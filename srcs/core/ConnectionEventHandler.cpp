
#include "Connection.hpp"
#include <iostream>

namespace core {
    void Connection::on_event( io::EventType event ) {
        // handle connection event

        std::cout << "Connection event: " << event << '\n';
    }
}


#include "Connection.hpp"
#include <iostream>

namespace core {
    void Connection::on_event( uint32_t events ) {
        // handle connection event

        std::cout << "Connection event: " << events << '\n';
    }
}

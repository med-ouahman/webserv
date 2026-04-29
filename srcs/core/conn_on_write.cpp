
#include "Connection.hpp"
#include <cerrno>
#include <cerrno>
#include <string.h>

namespace core {

    void Connection::on_readable( void ) {
        read();
        /* handle the logic here >> */
    }

    void Connection::on_writeable( void ) {
        write();
        /* naa */
    }
}

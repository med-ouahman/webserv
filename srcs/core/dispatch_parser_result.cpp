#include "Connection.hpp"

namespace core {

    void Connection::on_client_error() {
        std::cout << "Error\n";
        dispatcher.build_error_response(http::BAD_REQUEST, "Bad request");
        state = ConnectionState::WRITING;

        close_after_write = true;
    }
    
}

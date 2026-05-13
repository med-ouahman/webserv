#include "Connection.hpp"

namespace core {

    void Connection::process_outgoing_data() {

        if (writer.offset() < writer.size())
            return ;

        writer.reset();

        ssize_t produced = response.produce(&writer);
        std::cout << "X: " << produced << "\n";
        if (produced < 0) {
            state = ConnectionState::CLOSING;
            processing = false;
            return ;
        }
        
        if (produced == 0) {
            if (close_after_write) state = ConnectionState::CLOSING;
            else if (!data_view.empty()) state = ConnectionState::READING;
        }
        
    }


    void Connection::on_write_complete() {
        std::cout << "Write complete\n";
        /// nothing for now, the code above handles it??
    }

    void Connection::on_write_error() {
        processing = false;
        state = ConnectionState::CLOSING;
    }

}

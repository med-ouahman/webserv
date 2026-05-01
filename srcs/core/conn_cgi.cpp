
#include "Connection.hpp"

namespace core {

    void Connection::enter_cgi( const http::CGIContext& cgi_ctx ) {
        
        cgi_handler = new http::CGIHandler(*this, loop);
        try {
            cgi_handler->spawn(cgi_ctx);
        } catch (std::runtime_error& err) {
            std::cerr << err.what() << "\n";
            exit_cgi();
            dispatcher.build_error_response(http::INTERNAL_SERVER_ERROR, "Internal Server Error");
            state = ConnectionState::WRITING;
            close_after_write = true;
        }


    }

    void Connection::exit_cgi( void ) {
        delete cgi_handler;
        cgi_handler = NULL;
    }

}
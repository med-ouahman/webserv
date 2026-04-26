
#include "Connection.hpp"

namespace core {

    void Connection::enter_cgi( const http::CGIContext& cgi_ctx ) {
        cgi_handler = new http::CGIHandler(loop, *this);

        cgi_handler->spawn(cgi_ctx);
    }

    void Connection::exit_cgi( void ) {
        delete cgi_handler;
        cgi_handler = NULL;
    }

}
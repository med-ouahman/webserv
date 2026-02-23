#pragma once

#include "ConnectionState.hpp"
#include "ConnectionAction.hpp"
#include "IOHandler.hpp"
#include "ConnectionEvent.hpp"
#include "HTTPParser.hpp"
#include "HTTPResponseHandler.hpp"

namespace config {
    struct ServerConfig;
}

namespace core {
    class Connection: public io::IOHandler {
        private:
            int fd;
            ConnectionState state;
            http::HTTPParser p;
            http::HTTPResponseHandler handler;
            const config::ServerConfig* server_conf;
        public:
            explicit Connection( int fd );
            ~Connection();
            int get_fd() const;
            void on_close( void );
            ConnectionAction desired_action() const;
            void on_event( io::EventType event );
            bool on_bytes( char* buff );
            void queue_response( std::string const& response_buff );
    };
}

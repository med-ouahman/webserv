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
            uint32_t event_mask;
        private:
            ConnectionState state;
            http::HTTPParser p;
            http::HTTPResponseHandler handler;
            const config::ServerConfig* server_conf;
        private:
            std::string response_buff;
            size_t sent_offset;
        public:
            explicit Connection( int fd, const config::ServerConfig* conf, uint32_t mask );
            ~Connection();
        public:
            int get_fd() const;
            ConnectionAction desired_action() const;
            void on_event( io::EventType event );
            bool on_bytes( char* buff );
            void queue_response( void );
            uint32_t get_mask( void ) const { return event_mask; }
            void set_mask( uint32_t new_mask ) { event_mask = new_mask; }
        public:
            void update_buff( size_t sent_bytes );
            const std::string& get_write_buff( void ) const;
            size_t get_sent_offset( void ) const;
    };
}

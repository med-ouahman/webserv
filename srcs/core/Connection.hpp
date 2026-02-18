#pragma once

#include "ConnectionState.hpp"
#include "ConnectionAction.hpp"
#include "IOHandler.hpp"
#include "ConnectionEvent.hpp"
#include "HTTPParser.hpp"

namespace core {
    class Connection: public io::IOHandler {
        private:
            int fd;
            ConnectionState state;
            std::string s;
            http::HTTPParser p;            
        public:
            explicit Connection( int fd );
            ~Connection();
            int get_fd() const;
            void on_close( void );
            ConnectionAction desired_action() const;
            void on_event( io::EventType event );
            bool on_bytes( char* buff );
            size_t peek_bytes( char* buff, size_t size );
            void consume_bytes( size_t size );
    };
}

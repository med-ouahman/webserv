#pragma once

#include "ConnectionState.hpp"
#include "ConnectionAction.hpp"
#include "IOHandler.hpp"
#include "ConnectionEvent.hpp"
#include "HTTPParser.hpp"
#include "HTTPResponseHandler.hpp"
#include "ConnectionStateMachine.hpp"
#include "CGIHandler.hpp"

namespace config {
    struct ServerConfig;
}

namespace core {

    class Connection: public io::IOHandler {
        public:
            explicit Connection( int fd, const config::ServerConfig* conf, uint32_t mask, const io::EventLoop& loop );
            ~Connection();
            const static std::size_t READ_BUFFER_SIZE = 1024 * 16;

        private:
            const static std::size_t SEND_CHUNK_SIZE = 1024 * 16;
            const static std::size_t MAX_REQUESTS = 100;
        
        private:
            /* epoll */
            int fd;
            ::uint32_t event_mask;

        private:
            /* config + parsing + response generation */
            ConnectionState state;
            http::HTTPParser p;
            http::HTTPResponseHandler handler;
            const config::ServerConfig* server_conf;
            bool close_after_write;
            ::size_t num_requests;
            
        private:
            /* Output */
            char output_buff[SEND_CHUNK_SIZE];
            ::ssize_t bytes_in_buff;
            ::size_t sent_offset;
        private:
            /* input */
            char read_buff[READ_BUFFER_SIZE];
            ::size_t bytes_received;
            bool read_buff_drained;

        private:
            /* CGI */
            http::CGIHandler cgi_handler;
            
        private:
            bool progress;

        private:
            bool advance( void );
    
        public:
            int get_fd( void ) const;
            ConnectionAction desired_action() const;
            void on_event( io::EventType event );
            bool on_bytes( void);
            ::uint32_t get_mask( void ) const { return event_mask; }
            void set_mask( uint32_t new_mask ) { event_mask = new_mask; }
        
        public:
            bool has_data( ::ssize_t sent_bytes );
            const char* get_write_buff( void ) const;
            ::size_t bytes_remaining( void ) const;
            bool set_readbuff( ::ssize_t bytes );
            bool read_buff_empty() const;
            char* get_read_buff( void ) const {
                return (char* )read_buff;
            }
    };
}

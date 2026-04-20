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
            explicit Connection( int fd, const config::Config& conf, uint32_t mask, const io::EventLoop& loop );
            ~Connection();
            const static std::size_t READ_BUFFER_SIZE = 1024 * 16;

        private:
            const static std::size_t SEND_CHUNK_SIZE = 1024 * 16;
            const static std::size_t MAX_REQUESTS = 100;
            const static std::size_t MAX_INACTIVITY_LIMIT = 100;
            const static std::size_t MAX_CGI_WAIT = 2000;
            const static std::size_t MAX_IDLE_TICKS = 500;
            const static std::size_t MIN_PROGRESS_BYTES = 1024 * 4; /* how many bytes are considered progress */
            const static std::size_t MIN_BODY_CHUNK       = 4096;
            const static std::size_t REQUEST_LINE_LIMIT_TICKS   = 100;
            const static std::size_t HEADERS_LIMIT_TICKS  = 500;
            const static std::size_t BODY_LIMIT_TICKS     = 1000;
            
        private:
            /* epoll */
            int fd;
            uint32_t event_mask;

        private:
            /* config + parsing + response generation */
            ConnectionState::Type state;
            http::HTTPParser p;
            http::HTTPResponseHandler handler;
            const config::Config& config;
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
        
        private:
            /* timeout */
            ::size_t bytes_parsed_since_progress; /**/
            uint64_t inactivity_ticks; /* if socket's alive read > 0 or write > 0 */
            uint64_t current_state_ticks; /* if a connection is trapped */
            uint64_t current_state_tick_limit;

        private:
            /* CGI */
            http::CGIHandler cgi_handler;

        private:
            bool advance( void );
    
        public:
            int get_fd( void ) const;
            ConnectionAction desired_action() const;
            void on_event( io::EventType event );
            bool process_incoming_data( void );
            ::uint32_t get_mask( void ) const { return event_mask; }
            void set_mask( uint32_t new_mask ) { event_mask = new_mask; }
        
        public:
            bool on_write( ::ssize_t sent_bytes );
            const char* get_write_buff( void ) const;
            ::size_t bytes_remaining( void ) const;
            bool on_read( ::ssize_t bytes );
            bool read_buff_empty() const;
            char* get_read_buff( void ) const {
                return (char* )read_buff;
            };

            void tick( void );
    };
}

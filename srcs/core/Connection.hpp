#pragma once

#include "ConnectionState.hpp"
#include "ConnectionAction.hpp"
#include "ConnectionEvent.hpp"
#include "HTTPParser.hpp"
#include "CGIHandler.hpp"
#include "CGIContext.hpp"
#include "HTTPDispatcher.hpp"
#include "Stream.hpp"

namespace config {
    struct ServerConfig;
}

namespace core {

    class Connection: public io::Stream {
        public:
            explicit Connection( int fd, const config::Config& conf, uint32_t mask, const io::EventLoop& loop );
            ~Connection();

        private:
            const static std::size_t SEND_CHUNK_SIZE = 1024 * 16;
            const static std::size_t MAX_REQUESTS = 100;
            const static std::size_t MAX_INACTIVITY_LIMIT = 100;
            const static std::size_t MAX_IDLE_TIMEOUT = 500;
            const static std::size_t MIN_PROGRESS_BYTES = 1024 * 4;
            const static std::size_t MIN_BODY_CHUNK       = 4096;
            const static std::size_t REQUEST_LINE_LIMIT_TICKS   = 100;
            const static std::size_t HEADERS_LIMIT_TICKS  = 500;
            const static std::size_t BODY_LIMIT_TICKS     = 1000;
            
        private:
            /* epoll */
            int fd;
            uint32_t event_mask;
            const io::EventLoop& loop;
            
        private:
            /* config + parsing + response generation */
            ConnectionState::Type state;
            http::HTTPParser p;
            http::HTTPDispatcher dispatcher;
            const config::Config& config;
            bool close_after_write;
            ::size_t num_requests;

        private:
            /* timeout */
            uint64_t ms_;

        private:
            /* CGI */
            http::CGIHandler* cgi_handler;
            void enter_cgi( const http::CGIContext& cgi_ctx );
            void exit_cgi( void );
            
        private:
            void process( void );
            void error( void );
            bool advance( void );
    
        public:
            int get_fd( void ) const;
            ConnectionAction desired_action() const;
            void set_mask( uint32_t new_mask ) { event_mask = new_mask; }
            uint32_t get_mask( void ) const { return event_mask; }
            
        public:
            bool process_incoming_data( void ); // soon be private.
            bool on_write( void );
            bool on_read( void );
            void tick( void );
    };
}

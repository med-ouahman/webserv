#pragma once

#include "ConnectionState.hpp"
#include "ConnectionAction.hpp"
#include "ConnectionEvent.hpp"
#include "HTTPParser.hpp"
#include "CGIHandler.hpp"
#include "CGIContext.hpp"
#include "HTTPDispatcher.hpp"
#include "HTTPResponse.hpp"
#include "Stream.hpp"
#include "BodyParser.hpp"
#include "DataView.hpp"

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
            bool processing;
            uint32_t event_mask;
            const io::EventLoop& loop;
            
        private:
            /* config + parsing + response generation */

            void on_client_error();
            void on_request_ready();

            DataView view;
            ConnectionState::Type state;
            http::HTTPParser p;
            http::BodyParser body_p;
            http::HTTPDispatcher dispatcher;
            const config::Config& config;
            bool close_after_write;
            ::size_t num_requests;
            http::HTTPResponse response;

        private:
            /* timeout */
            uint64_t ms_;

        private:
            /* CGI */
            http::CGIHandler* cgi_handler;
            void enter_cgi( const http::CGIContext& cgi_ctx );
            void exit_cgi();
            
        private:
            bool process();
            bool advance();
            bool readbuf_drained() { return view.len_ == view.bytes_consumed; }
    
        public:
            int get_fd() const;
            ConnectionAction desired_action() const;
            void set_mask( uint32_t new_mask ) { event_mask = new_mask; }
            uint32_t get_mask() const { return event_mask; }
            http::HTTPResponse& get_response() { return response; }
            void tick();
            
        private:
            void process_outgoing_data();
            bool process_incoming_data();
            void handle_event();
            void on_writeable();
            void on_readable();

    };
}

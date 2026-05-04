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
#include "BodyHandler.hpp"
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
            uint32_t event_mask;
            const io::EventLoop& loop;
            
        private:
            /* config + parsing + response generation */
            ConnectionState::Type state;
            http::HTTPParser p;
            http::BodyHandler body_p;
            http::HTTPDispatcher dispatcher;
            const config::Config& config;
            bool close_after_write;
            size_t num_requests;
            http::HTTPResponse response;
            ConnectionAction::Type action;

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
            bool readbuf_drained() { return data_view.len_ == data_view.bytes_consumed; }
            void on_client_error();
            void on_request_ready();
    
        public:
            int get_fd() const;
            ConnectionAction::Type desired_action() const;
            void set_mask( uint32_t new_mask ) { event_mask = new_mask; }
            uint32_t get_mask() const { return event_mask; }
            http::HTTPResponse& get_response() { return response; }
            void tick();
            void on_cgi_finished();
            
        private:
            void process_outgoing_data();
            void process_incoming_data();
            void handle_event();
            void on_read_eof();
            void on_write_complete();
            void on_write_error();
    };
}

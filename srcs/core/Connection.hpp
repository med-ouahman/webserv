#pragma once

#include "ConnectionState.hpp"
#include "ConnectionAction.hpp"
#include "HTTPParser.hpp"
#include "CGIHandler.hpp"
#include "CGIContext.hpp"
#include "HTTPDispatcher.hpp"
#include "HTTPResponse.hpp"
#include "Stream.hpp"
#include "BodyHandler.hpp"
#include "DataView.hpp"
#include "IRequestHandler.hpp"

namespace config {
    struct ServerConfig;
}

namespace core {

    class Connection: public io::Stream {
        private:
            const static std::size_t SEND_CHUNK_SIZE            = 16384;
            const static std::size_t MAX_REQUESTS               = 100;
            const static std::size_t MAX_INACTIVITY_LIMIT       = 100;
            const static std::size_t MAX_IDLE_TIMEOUT           = 500;
            const static std::size_t MIN_PROGRESS_BYTES         = 4096;
            const static std::size_t MIN_BODY_CHUNK             = 4096;
            const static std::size_t REQUEST_LINE_LIMIT_TICKS   = 100;
            const static std::size_t HEADERS_LIMIT_TICKS        = 500;
            const static std::size_t BODY_LIMIT_TICKS           = 1000;
            
        private:
            uint32_t event_mask;
            ConnectionState::Type state;
            RequestPhase::Type phase;
            http::HTTPParser p;
            http::BodyHandler body_handler;
            bool close_after_write;
            size_t num_requests;
            
            http::HTTPResponse response;
            http::CGIHandler* cgi_handler;
            http::IRequestHandler* request_handler;
            
        public:
            http::ResolutionResult current_res;
            io::EventLoop& loop;
            explicit Connection( int fd, uint32_t mask, io::EventLoop& loop );
            ~Connection();
            int get_fd() const;
            void invoke_cgi( const http::CGIContext& context );
            void release_cgi_handler();
            void on_cgi_finished();
            void bind_cgi();
            void on_cgi_output_ready();
            void on_cgi_error( http::HTTPStatusCode c, std::string const& reason );
            ConnectionAction desired_action() const;
            void set_mask( uint32_t new_mask ) { event_mask = new_mask; }
            uint32_t get_mask() const { return event_mask; }
            http::HTTPResponse& get_response() { return response; };
            http::BodyHandler& get_body_handler() { return body_handler;};

        private:
            void on_client_error();
            void process();
            void process_outgoing_data();
            void process_incoming_data();
            void handle_event();
            void on_read_eof();
            void on_read_error();
            void on_write_complete();
            void on_write_error();

            void request_building();
            void request_resloving();
            void request_processing();
            void request_reading_body();
    };
}

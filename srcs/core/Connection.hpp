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
#include "Timestamp.hpp"
#include "limits.hpp"

namespace config {
    struct ServerConfig;
}

namespace core {

    class Connection: public io::Stream {
        private:
            uint32_t event_mask;
            ConnectionState::Type state;
            RequestPhase::Type phase;
            http::HTTPParser p;
            http::BodyHandler body_handler;
            bool close_after_write;
            size_t num_requests;
            size_t body_bytes_received;
            http::HTTPResponse response;
            http::IRequestHandler* request_handler;
            Timestamp last_;
            
        public:
            Timestamp& last() { return last_; };
            http::ResolutionResult current_res;
            io::EventLoop& loop;
            explicit Connection( int fd, uint32_t mask, io::EventLoop& loop );
            ~Connection();
            void release_cgi_handler();
            void on_cgi_finished();
            void bind_cgi();
            void on_cgi_output_ready();
            void on_cgi_error( http::HTTPStatusCode c, std::string const& reason );
            bool timedout();
            ConnectionAction action() const;
            void set_mask( uint32_t new_mask ) { event_mask = new_mask; }
            uint32_t get_mask() const { return event_mask; }
            http::HTTPResponse& get_response() { return response; };
            http::BodyHandler& get_body_handler() { return body_handler;};
            RequestPhase::Type request_phase() const { return phase; }

        private:
            void on_client_error();
            void process();
            void process_incoming_data();
            void handle_event();
            void on_read_eof();
            void on_read_error();
            void process_outgoing_data();
            void on_write_complete();
            void on_write_error();
            /* */
            void request_building();
            void request_resloving();
            void request_processing();
            void request_reading_body();
    };
}

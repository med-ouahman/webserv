#pragma once

#include "ConnectionState.hpp"
#include "ConnectionAction.hpp"
#include "Timestamp.hpp"
#include "Limits.hpp"
#include "HttpSession.hpp"

namespace config {
    struct ServerConfig;
}

namespace core {

    class Connection: public io::Stream {
        
        private:
            ConnectionState state;
           
            bool close_after_write;
            size_t num_requests;
            
            Timestamp last_;
            Timestamp conn_lifetime;
            http::HttpSession session;
        
        public:
            io::EventLoop& loop;
            explicit Connection( int fd, io::EventMask mask, io::EventLoop& loop );
            ~Connection();
            void release_cgi_handler();
            void on_cgi_finished();
            void bind_cgi();
            void on_cgi_output_ready();
            void on_cgi_error( http::StatusCode c );
            bool timedout();
            ConnectionAction action() const;
            
        private:
            void handle_event();
    
    };
}

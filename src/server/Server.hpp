#ifndef SERVER_HPP
#define SERVER_HPP


#include "EventLoop.hpp"
#include "Connection.hpp"
#include "Listener.hpp"

class Server {

    private:
        bool running_;

        std::vector<net::Connection*> connections;
        std::vector<net::Listener*> listeners;

        runtime::epoll::EventLoop event_loop;
        
        Server(const Server& );
        Server& operator=( const Server& );

        bool start_listeners();
        void add_connection( int client_fd );
        void remove_connection( net::Connection* conn );
        
    public:
        Server();
        ~Server();
        int start();
        static void server_disconnect( net::ServerContext ctx, net::Connection* conn );
        static void server_accept( int conn_fd, net::AcceptContext server );
};

#endif

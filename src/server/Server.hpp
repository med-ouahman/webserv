#ifndef SERVER_HPP
#define SERVER_HPP


#include "EventLoop.hpp"
#include "Connection.hpp"
#include "ListeningSocket.hpp"

class Server {

    private:
        std::vector<net::Connection*> connections;
        Server(const Server& );
        Server& operator=( const Server& );

    public:
        Server();
        ~Server();
        int start();
};

#endif

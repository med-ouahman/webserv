#include "EventLoop.hpp"
#include "Connection.hpp"

namespace io {

    void EventLoop::sweep() {

        for ( size_t i(0); i < conns.size(); ) {
            bool drop = conns[i]->action().want_close;
            std::cout << (drop?"want close\n":"");
            drop = drop or conns[i]->timedout();
            if (drop)
            {
                delete_fd(conns[i]->fd());
                delete conns[i];
                conns.erase(conns.begin() + i);
            }

            else ++i;
        }
       
        for ( size_t i(0); i < cgi_bin.size(); ) {

            bool drop = cgi_bin[i]->finished();
            if (drop) std::cout << "CGI FINISHED\n";
            drop = drop or cgi_bin[i]->timedout();
            if (drop) {
                delete cgi_bin[i];
                cgi_bin.erase(cgi_bin.begin() + 1);
            } else ++i;
        }

    }


}

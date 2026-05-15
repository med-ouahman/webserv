#include "EventLoop.hpp"
#include "Connection.hpp"
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <sys/epoll.h>
#include "Timestamp.hpp"

namespace io {

    bool EventLoop::add_connection( int client_fd ) {

        conns.push_back(new core::Connection(client_fd, EPOLLIN, *this));
        if (not add_fd(client_fd, EPOLLIN, conns.back())) {
            conns.pop_back();
            return false;
        }

        return true;
    }


    void EventLoop::update_epoll_interest( core::Connection* conn ) {

        uint32_t new_mask = EPOLLIN;
        core::ConnectionAction action = conn->action();
        
        if (action.want_write) {
            new_mask = EPOLLOUT;
        } else if (action.want_close) {
            return ;
        }
        
        if (new_mask != conn->get_mask()) {
            std::cout << "Connection WANTS: " << (action.want_write ? "WRITING\n":"READING\n");
            mod_fd(conn->fd(), new_mask, conn);
            conn->set_mask(new_mask);
        }
    }

    void EventLoop::add_cgi_handler( http::CGIHandler* h ) {
        cgi_bin.push_back(h);
    }


}

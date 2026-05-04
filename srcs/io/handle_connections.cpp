#include "EventLoop.hpp"
#include "Connection.hpp"
#include <fcntl.h>
#include <cerrno>
#include <cstring>

namespace io {

    bool EventLoop::add_connection( int client_fd ) {

        conns.push_back(new core::Connection(client_fd, conf, EPOLLIN | EPOLLET, *this));
        if (!add_fd(client_fd, EPOLLIN | EPOLLET, conns.back())) {
            conns.pop_back();
            return false;
        }

        return true;
    }

    bool EventLoop::remove_connections() {

        for ( size_t i(0); i < conns.size(); ) {
            if (conns[i]->desired_action() == core::ConnectionAction::CLOSE) {
                del_fd(conns[i]->get_fd());
                delete conns[i];
                conns.erase(conns.begin() + i);
            } else {
                ++i;
            }
        }
       
        return true;
    }

    void EventLoop::update_epoll_interest( core::Connection* conn ) {

        uint32_t new_mask = EPOLLIN | EPOLLET;
        core::ConnectionAction::Type action = conn->desired_action();
        
        switch (action) {

            case core::ConnectionAction::READ:
                break;
            case core::ConnectionAction::DISABLE_READ: case core::ConnectionAction::DISABLE_WRITE:
                del_fd(conn->get_fd());
                return ;
            case core::ConnectionAction::WRITE:
                new_mask = EPOLLOUT | EPOLLET;
                break;
            case core::ConnectionAction::CLOSE:
                return ;
        }
        
        if (new_mask != conn->get_mask()) {
            mod_fd(conn->get_fd(), new_mask, conn);
            conn->set_mask(new_mask);
        }
    }
}

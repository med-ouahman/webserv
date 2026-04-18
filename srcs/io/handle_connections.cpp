#include "EventLoop.hpp"
#include "Connection.hpp"
#include <fcntl.h>
#include <cerrno>
#include <cstring>

namespace io {

    bool EventLoop::add_connection( int client_fd ) {

        int flags = ::fcntl(client_fd, F_GETFL);
        if (flags < 0) {
            return false;
        }
        
        ::fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
       
        conns.push_back(new core::Connection(client_fd, &conf.server, EPOLLIN | EPOLLET, *this));

        if (!add_fd(client_fd, EPOLLIN | EPOLLET, conns.back())) {
            conns.pop_back();
            return false;
        }
        return true;
    }

    bool EventLoop::remove_connections( void ) {

        for ( ::size_t i = 0; i < conns.size(); ) {
            if (conns[i]->desired_action().want_close) {
                delete conns[i];
                conns.erase(conns.begin() + i);
            } else {
                ++i;
            }
        }
       
        return true;
    }

    bool EventLoop::apply_connection_actions( core::Connection* conn ) {
        
        core::ConnectionAction action = conn->desired_action();
        
        if (action.want_read) {
            read_from_socket(*conn);
            action = conn->desired_action();
        }
        
        if (action.want_write) {
            write_to_socket(*conn);
            action = conn->desired_action();
        }

        return action.want_process;
    }

    void EventLoop::update_epoll_interest( core::Connection* conn ) {

        ::uint32_t new_mask = EPOLLIN | EPOLLET;
        core::ConnectionAction action = conn->desired_action();
        
        if (action.want_process) {
            return ;
        }
        if (action.want_write) {
            new_mask = EPOLLOUT | EPOLLET;
        }
        if (action.want_close) {
            return ;
        }

        if (new_mask != conn->get_mask()) {
            mod_fd(conn->get_fd(), new_mask, conn);
            conn->set_mask(new_mask);
        }
    }
}

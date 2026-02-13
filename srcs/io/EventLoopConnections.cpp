#include "EventLoop.hpp"
#include "Connection.hpp"
#include <fcntl.h>

namespace io {
    bool EventLoop::add_connection( int client_fd ) {
        core::Connection* conn = new core::Connection(client_fd);
        int flags = fcntl(client_fd, F_GETFL);
        fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
        if (add_fd(client_fd, EPOLLIN | EPOLLET | EPOLLOUT, conn)) {
            conns.push_back(*conn);
            return true;
        }
        return false;
    }

    bool EventLoop::remove_connection( core::Connection* conn ) {

        del_fd(conn->get_fd());
        size_t i = 0;

        for ( ; i < conns.size(); i++ ) {
            if (&conns[i] == conn) {
                break;
            }
        }
        
        conns[i] = conns.back();
        conns.pop_back();
        return true;
    }

    void EventLoop::apply_connection_actions( core::Connection* con ) {
        core::ConnectionAction action = con->desired_action();
        if (action.want_close) {
            //
        } else if (action.want_read) {
            //
        } else if (action.want_write) {
            //
        }
    }
}

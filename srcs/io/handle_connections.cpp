#include "EventLoop.hpp"
#include "Connection.hpp"
#include <fcntl.h>
#include <cerrno>
#include <cstring>

namespace io {

    bool EventLoop::add_connection( int client_fd ) {

        int flags = ::fcntl(client_fd, F_GETFL);

        if (flags < 0 || ::fcntl(client_fd, F_SETFL, flags | O_NONBLOCK)) {
            LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::add_connection::fcntl()"));
            return false;
        }

        flags = ::fcntl(client_fd, F_GETFD);

        if (flags < 0 || ::fcntl(client_fd, F_SETFD, flags | O_CLOEXEC)) {
            LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::add_connection::fcntl()"));
            return false;
        }
        
        conns.push_back(new core::Connection(client_fd, conf, EPOLLIN | EPOLLET, *this));
        if (!add_fd(client_fd, EPOLLIN | EPOLLET, conns.back())) {
            conns.pop_back();
            return false;
        }

        return true;
    }

    bool EventLoop::remove_connections( void ) {

        for ( ::size_t i(0); i < conns.size(); ) {
            if (conns[i]->desired_action().want_close) {
                del_fd(conns[i]->get_fd());
                delete conns[i];
                conns.erase(conns.begin() + i);
            } else {
                ++i;
            }
        }
       
        return true;
    }

    bool EventLoop::apply_connection_actions( core::Connection* conn ) {
        
        conn->tick();
        core::ConnectionAction action = conn->desired_action();


        if (action.want_read) {
            read_from_socket(*conn);
        } else if (action.want_write) {
            write_to_socket(*conn);
        } else if (action.want_cgi) {
            http::CGIHandler* cgi_handler = conn->get_cgi_handler();
            
            ReadFd stderr_fd = cgi_handler->get_stderr_pipe();
            add_fd(stderr_fd.fd, EPOLLIN | EPOLLET, cgi_handler);
            
            ReadFd stdout_fd = cgi_handler->get_stdout_pipe();
            add_fd(stdout_fd.fd, EPOLLIN | EPOLLET, cgi_handler);
            
            WriteFd stdin_fd = cgi_handler->get_stdin_pipe();
            add_fd(stderr_fd.fd, EPOLLOUT | EPOLLET, cgi_handler);
            return true;
        }
        
        return action.want_process && !action.want_close;
    }

    void EventLoop::update_epoll_interest( core::Connection* conn ) {

        ::uint32_t new_mask = EPOLLIN | EPOLLET;
        core::ConnectionAction action = conn->desired_action();
        
        if (action.want_process || action.want_close) {
            return ;
        }

        if (action.want_write) {
            new_mask = EPOLLOUT | EPOLLET;
        }

        if (new_mask != conn->get_mask()) {
            mod_fd(conn->get_fd(), new_mask, conn);
            conn->set_mask(new_mask);
        }
    }
}

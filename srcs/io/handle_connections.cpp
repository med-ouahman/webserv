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

    void EventLoop::sweep() {

        for ( size_t i(0); i < conns.size(); ) {
            bool drop = conns[i]->action().want_close;
            std::cout << (drop?"want close\n":"");
            drop = drop or timedout(conns[i]);
            if (drop)
            {
                del_fd(conns[i]->get_fd());
                delete conns[i];
                conns.erase(conns.begin() + i);
            }

            else ++i;
        }
       
        for ( size_t i(0); i < cgi_bin.size(); ++i ) {
            delete cgi_bin[i];
        }

        cgi_bin.clear();
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
            mod_fd(conn->get_fd(), new_mask, conn);
            conn->set_mask(new_mask);
        }
    }

    void EventLoop::add_cgi_handler( http::CGIHandler* h ) {
        cgi_bin.push_back(h);
    }

    bool EventLoop::timedout( core::Connection* conn ) {
        
        double limit = limits::MAX_IDLE_TIMEOUT;
       
        switch (conn->request_phase()) {
            case core::RequestPhase::INITIAL:
                limit = limits::MAX_INITIAL_TIMEOUT;
                break;
            case core::RequestPhase::BUILDING:
                limit = limits::MAX_HEADER_TIMEOUT;
                break;
            case core::RequestPhase::READING_BODY:
                limit = limits::MAX_BODY_PROGRESS_TIMEOUT;
                break;
            case core::RequestPhase::IDLE:
                limit = limits::MAX_IDLE_TIMEOUT;
            default:
                limit = 0;
        }

        if (conn->last().elapsed() >= limit) {
            return true;
        }

        return false;
    }

}

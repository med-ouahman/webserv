#pragma once

#include <vector>

namespace config { struct Config; }
namespace logger { class Logger; }
namespace http { class SessionManager; }

namespace runtime { namespace epoll { class EventLoop; } }

struct RuntimeServices {

    runtime::epoll::EventLoop& poller;
    logger::Logger& logger;
    std::vector<http::SessionManager*>& sessions;

    RuntimeServices(
        runtime::epoll::EventLoop& p,
        logger::Logger& l,
        std::vector<http::SessionManager*>& s):
        poller(p),
        logger(l),
        sessions(s) {}

};

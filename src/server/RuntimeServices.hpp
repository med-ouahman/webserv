#pragma once

namespace config { struct Config; }
namespace logger { class Logger; }

namespace runtime { namespace epoll { class EventLoop; } }

struct RuntimeServices
{
runtime::epoll::EventLoop& poller;
logger::Logger& logger;
const config::Config& conf;

RuntimeServices(runtime::epoll::EventLoop& p, logger::Logger& l, const config::Config& c)
:
poller(p),
logger(l),
conf(c) {}

};

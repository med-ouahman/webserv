#pragma once

namespace http {
	class Context;
}

namespace logger { class Logger; }
namespace runtime { namespace epoll { class EventLoop; }}

struct ServerContext {
	runtime::epoll::EventLoop* poller;
	logger::Logger* logger;
};


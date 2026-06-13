#pragma once

namespace http {
	class Context;
}

namespace logger { class Logger; }
namespace runtime { namespace epoll { class EventPoller; }}

struct ServerContext {
	runtime::epoll::EventPoller* poller;
	logger::Logger* logger;
};


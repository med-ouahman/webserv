#pragma once

namespace http { class Context; }
namespace config { struct Config; }
namespace logger { class Logger; }
namespace runtime { namespace epoll { class EventLoop; } }

struct ServerContext {
	runtime::epoll::EventLoop* poller;
	logger::Logger* logger;
	const config::Config& conf;
	ServerContext(const config::Config& c): conf(c) {}
};

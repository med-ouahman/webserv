#pragma once

#include "AEventHandler.hpp"
#include "Result.hpp"
#include <vector>
#include "Config.hpp"

#ifndef NDEBUG
#define NDEBUG 4
#endif
#include <cassert>
#define BACKLOG 5

namespace net {

enum ListenerState {
	LISTENING,
	LISTENER_ERROR
};

typedef void (*AcceptCallback)(int client_fd, void* server_ctx);

struct AcceptContext {
	void* server_ctx;
	AcceptCallback callback;
};

class Listener: public io::AEventHandler {

private:
	ListenerState state_;
	Listener(const Listener& socket);
	Listener& operator=(const Listener& socket);
	bool accept_clients();
	bool on_error();
	AcceptContext accept_ctx;

public:
	explicit Listener(int fd, io::Event mask, AcceptContext ctx);
	~Listener();
	void on_event(io::Event event);
	bool error() const;
};

Base::Result<Listener*>
create_listening_socket(
	const config::ListenEndPoint& endpoints,
	AcceptContext ctx);

}

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

	typedef void* AcceptContext;

	typedef void (*AcceptCallback)( int client_fd, AcceptContext context );

	class ListeningSocket: public io::AEventHandler {
		private:
			ListeningSocket& operator=( const ListeningSocket& socket );
			bool accept_clients();
			bool on_error();

			AcceptCallback callback_;
			AcceptContext context_;


		public:
			ListeningSocket( const ListeningSocket& socket );
			explicit ListeningSocket( int fd, io::EventMask mask, AcceptCallback cb, AcceptContext ctx );
			~ListeningSocket();
			void on_event( runtime::epoll::Event event );
	};

	Base::Result<ListeningSocket*>
	create_listening_socket(
		const config::ListenEndPoint& endpoints,
		AcceptCallback cb,
		AcceptContext ctx );
}

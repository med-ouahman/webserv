#pragma once

#include "BodyHandler.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Parser.hpp"
#include "IRequestHandler.hpp"

namespace http {

	enum State {

	};

	class HttpSession {
		private:
			Request request;
			Response response;
			BodyHandler body_handler;
			IRequestHandler* request_handler;
			Dispatcher dispatcher;

			bool keep_alive_;

			HttpSession( const HttpSession& );
			HttpSession& operator=( const HttpSession& );

			State state;

		public:
			HttpSession();
			~HttpSession();

			void consume( DataView& data_view );

			void produce( BufferWriter& writer );

			void reset();
			bool should_keep_alive();


	};

}

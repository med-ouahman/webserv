#pragma once

#include "BodyReader.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Parser.hpp"
#include "IRequestHandler.hpp"
#include "Dispatcher.hpp"

namespace http {

	enum State {
		INITIAL,
		BUILDING,
		RESOLVING,
		READING_BODY,
		PROCESSING,
		WRITING_RESPONSE,
		FINAL,
		IDLE,
		SESSION_ERROR
	};

	class HttpSession {
		
		private:
			Parser				parser;
			Request				request;
			Response			response;
			BodyReader 			body_reader;
			IRequestHandler*	request_handler;
			Dispatcher			dispatcher;

			bool	keep_alive_;
			size_t	num_requests_;
			State	state;

			HttpSession( const HttpSession& );
			HttpSession& operator=( const HttpSession& );
			
		public:
			HttpSession();
			~HttpSession();

			void consume( DataView& data_view );

			void produce( BufferWriter& writer );

			void reset();

			bool should_keep_alive();
	};

}

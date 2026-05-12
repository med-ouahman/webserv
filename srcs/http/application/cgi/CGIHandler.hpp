#pragma once

#include "IOChannel.hpp"
#include "PipeGuard.hpp"
#include "LineScanner.hpp"
#include "IRequestHandler.hpp"

namespace io {
	class EventLoop;
}

namespace core {
	class Connection;
}


namespace http {

	class CGIState {
		public:
			enum Type {

				SPAWN,
				ACTIVE,
				WRITING_BODY,
				WAITING,
				FINISHED,
				ERROR,
			};
	};
	
	struct CGIContext;

	class CGIHandler: public IRequestHandler {
		
		public:
			const static std::size_t MAX_BLOCK_LEN = 1024 * 16; // 16KB
			const static std::size_t MAX_CGI_BODY_LEN = 1024 * 1024; // 1MB
			
			explicit CGIHandler( core::Connection& conn_ );
			void spawn( const io::EventLoop& loop, const CGIContext& context );
			void handle();
			bool done();
			~CGIHandler();

		private:
			CGIHandler( const CGIHandler& );
			CGIHandler& operator=( const CGIHandler& );
			static time_t cgi_timeout_ms;

		private:
			struct CGIOutputState {
				enum Type {
					
					STATUS_LINE,
					HEADERS,
					BODY,
					WRITING_BODY,
				};
			};
			
			CGIOutputState::Type output_state;
			CGIState::Type cgi_state;

			enum CGIError {
				NONE,
				TIMEOUT,
				CRASH,
				PIPE_FAIL,
			} cgi_error;

			pid_t	cgi_pid;
			int		cgi_status;

			PipeGuard pipe_guard;
			
			IOChannel stdin_ch;
			IOChannel stdout_ch;
			IOChannel stderr_ch;
	
			core::Connection& conn;
			core::DataView& stdout_ch_view;
			LineScanner scanner;

			time_t start_time;

		public:
			void on_channel_closed();
			ScanResult on_input_ready();
			ssize_t produce_output( core::BufferWriter* writer );
			void on_error();
			core::DataView& get_stdout_data_view();
			void pull(); // EXPIRED
			void on_ch_error();
			CGIState::Type get_cgi_state() const;
			bool finished();
			/**/

	};

}

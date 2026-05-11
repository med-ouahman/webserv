#pragma once

#include "IIOHandler.hpp"
#include "IOChannel.hpp"
#include <unistd.h>
#include <stdlib.h>
#include "PipeGuard.hpp"
#include "IDataListener.hpp"
#include "LineScanner.hpp"

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

	class CGIHandler: public io::IDataListener {
		
		public:
			const static std::size_t MAX_BLOCK_LEN = 1024 * 16; // 16KB
			const static std::size_t MAX_CGI_BODY_LEN = 1024 * 1024; // 1MB
			
			explicit CGIHandler( core::Connection& conn, const io::EventLoop& l );
			void spawn( const CGIContext& context );
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
			const io::EventLoop& loop;
			core::DataView& stdout_ch_view;
			LineScanner scanner;

			time_t start_time;

		public:
			void on_channel_closed();
			ScanResult on_input_ready();
			ssize_t produce_output( core::BufferWriter* writer );
			void on_error();
			core::DataView& get_stdout_data_view();
			void pull();
			void on_ch_error();
			CGIState::Type get_cgi_state() const;
			bool finished();
			/**/

	};

}

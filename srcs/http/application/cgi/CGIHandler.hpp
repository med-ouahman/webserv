#pragma once

#include "IOChannel.hpp"
#include "PipeGuard.hpp"
#include "LineScanner.hpp"
#include "IRequestHandler.hpp"
#include "Timestamp.hpp"
#include "HTTPDispatcher.hpp"

namespace io {
	class EventLoop;
}

namespace core {
	class Connection;
}


namespace http {

	struct ResolutionResult;

	class CGIState {
		public:
			enum Type {
				SPAWN,
				ACTIVE,
				WRITING_BODY,
				FINISHED,
				ERROR,
			};
	};
	
	struct CGIContext;

	class CGIHandler: public IRequestHandler {
		
		public:
			const static std::size_t MAX_BLOCK_LEN = 1024 * 16; // 16KB
			const static std::size_t MAX_CGI_BODY_LEN = 1024 * 1024 * 10; // 10MB
			
			explicit CGIHandler( core::Connection& conn_, const ResolutionResult result_ );
			void spawn( const io::EventLoop& loop );
			void handle();
			bool done();
			~CGIHandler();
			char** build_cgi_env( const CGIContext& ctx );
			char* transform( bool http_prefix, std::map<std::string, std::string>::iterator& it );
			std::map<std::string, std::string> build_cgi_metadata( const CGIContext& context );
			static bool forbidden_header( const std::string& header_name );

		private:
			CGIHandler( const CGIHandler& );
			CGIHandler& operator=( const CGIHandler& );
			static size_t cgi_timeout_secs;
			const static char* cgi_metadata[];
			const static char* stripped_headers[];

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
			const ResolutionResult result;
			DataView& stdout_ch_view;
			LineScanner scanner;

			Timestamp start_time;
			Timestamp sigterm_sent_at;

		public:
			void on_channel_closed();
			ScanResult on_input_ready();
			ssize_t produce_output( BufferWriter* writer );
			void on_error();
			DataView& get_stdout_data_view();
			void on_ch_error();
			bool finished();
			bool timedout();
			/**/

	};

}

#pragma once

#include "IOChannel.hpp"
#include "PipeGuard.hpp"
#include "LineScanner.hpp"
#include "IRequestHandler.hpp"
#include "Timestamp.hpp"
#include "Dispatcher.hpp"
#include "Headers.hpp"

namespace io {
	class EventLoop;
}

namespace core {
	class Connection;
}

namespace http {

	struct ResolutionResult;

	struct CGIState {
		enum Type {
			SPAWN,
			ACTIVE,
			WRITING_BODY,
			FINISHED,
			ERROR,
		};
	};

	struct CGIOutputState {
		enum Type {
			STATUS_LINE,
			HEADERS,
			BIND_BODY,
			WRITING_BODY,
			ERROR,
		};
	};
	
	struct CGIContext;

	struct CGIProcess {
		pid_t pid;
		int   status;
		Timestamp spawn_time;
		Timestamp sigterm_sent_at;
		IOChannel stdin_ch;
		IOChannel stdout_ch;
		IOChannel stderr_ch;
		private:
			PipeGuard p;
	};

	struct CGIParseContext {
		StatusCode status_code;
		std::string status_reason;
		bool	finished;
		size_t	bytes_parsed;
		CGIOutputState::Type state;
		Headers headers;

		CGIParseContext()
			: status_code(static_cast<StatusCode>(0)),
			finished(false),
			bytes_parsed(0),
			state(CGIOutputState::STATUS_LINE) {}
	};

	class CGIHandler: public IRequestHandler {

		private:
			static size_t cgi_timeout_secs;
			const static char* cgi_metadata[];
			const static char* stripped_headers[];
			
			char** build_cgi_env( const CGIContext& ctx );
			char* transform( bool http_prefix, Headers::const_iterator& it );
			Headers build_cgi_metadata( const CGIContext& context );
			static bool forbidden_header( const std::string& header_name );
			ScanResult parse_cgi_headers();
			void sanitize_status_line( const std::pair<std::string, std::string>& header );
			void sanitize_header( std::pair<std::string, std::string>& header );
			CGIHandler( const CGIHandler& );
			CGIHandler& operator=( const CGIHandler& );

			CGIState::Type cgi_state;
			/* CGI PROCESS */
			pid_t	cgi_pid;
			int		cgi_status;
			PipeGuard pipe_guard;
			IOChannel stdin_ch;
			IOChannel stdout_ch;
			IOChannel stderr_ch;
			core::Connection& conn;
			const ResolutionResult result;
			LineScanner scanner;
			Timestamp start_time;
			Timestamp sigterm_sent_at;
			CGIParseContext parse_ctx;

		public:
			const static std::size_t MAX_CGI_HEADER_BLOCK_LEN = 1024 * 4; 
			const static std::size_t MAX_CGI_BODY_LEN = 1024 * 1024 * 10; // 10MB
			explicit CGIHandler( core::Connection& conn_, const ResolutionResult result_ );
			void spawn( const io::EventLoop& loop );
			void handle();
			bool done();
			~CGIHandler();
			void on_channel_closed();
			ScanResult on_input_ready();
			ssize_t produce_output( BufferWriter* writer );
			void on_error();
			void on_ch_error();
			bool finished();
			bool timedout();
			DataView& stdout_ch_data_view();
			const Headers& cgi_headers() const;

	};

}

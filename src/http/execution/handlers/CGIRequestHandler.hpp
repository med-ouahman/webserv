#pragma once

#include "CGIProcess.hpp"

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

	class CGIRequestHandler: public IRequestHandler {

		private:
			const static char* cgi_metadata[];
			const static char* stripped_headers[];
			
			char** build_cgi_env( const CGIContext& ctx );
			char* transform( bool http_prefix, Headers::const_iterator& it );
			Headers build_cgi_metadata( const CGIContext& context );
			static bool forbidden_header( const std::string& header_name );
			ScanResult parse_cgi_headers();
			void sanitize_status_line( const std::pair<std::string, std::string>& header );
			void sanitize_header( std::pair<std::string, std::string>& header );
			CGIRequestHandler( const CGIRequestHandler& );
			CGIRequestHandler& operator=( const CGIRequestHandler& );

			CGIState::Type cgi_state;
			/* CGI PROCESS */
		
		
			core::Connection& conn;
			const ResolutionResult result;
			
			CGIParseContext parse_ctx;

		public:
			const static std::size_t MAX_CGI_HEADER_BLOCK_LEN = 1024 * 4; 
			const static std::size_t MAX_CGI_BODY_LEN = 1024 * 1024 * 10; // 10MB
			explicit CGIRequestHandler( core::Connection& conn_, const ResolutionResult result_ );
			
			void handle();
			bool done();
			~CGIRequestHandler();
			bool timedout();
			const Headers& cgi_headers() const;

	};

}

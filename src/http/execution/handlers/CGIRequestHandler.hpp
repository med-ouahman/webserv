#pragma once

#include "CGIProcess.hpp"
#include "IRequestHandler.hpp"

namespace http {

	struct ResolutionResult;

	struct CGIContext {

	};

	struct CGIParseContext {

	};

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

			cgi::CGIProcess process;
					
			CGIParseContext parse_ctx;

		public:
			const static std::size_t MAX_CGI_HEADER_BLOCK_LEN = 1024 * 4; 
			const static std::size_t MAX_CGI_BODY_LEN = 1024 * 1024 * 10; // 10MB
			explicit CGIRequestHandler( const ResolutionResult result_ );
			
			void handle();
			bool done();
			~CGIRequestHandler();
			bool timedout();
			const Headers& cgi_headers() const;
	};

}

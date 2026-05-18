
#pragma once 

#include <string>
#include <map>

namespace http {

	enum HTTPMethod
	{
		GET,
		POST,
		PUT,
		PATCH,
		DELETE,
		UNKNOWN
	};

	class RequestState {
		public:
		enum Type {

			REQUEST_LINE,
			HEADERS,
			BODY,
			DONE,
			REQUEST_ERROR
		};
	};

	struct RequestLine {
		HTTPMethod method;
		std::string version;
		std::string uri;
		
		RequestLine(): method(UNKNOWN), version(""), uri("") {}

		~RequestLine() {}
	};

	struct HTTPRequestData {
		HTTPMethod	method;
		std::string version;
		std::string unparsed_uri;
		std::string resource_path;
		std::string query_string;
		std::map<std::string, std::string> headers;
	};

	
	class HTTPRequest {
		
	private:
		static const size_t MAX_HEADER_COUNT = 100;

		HTTPRequestData data_;
	
		RequestState::Type request_state_;

		/* metadata */
		bool    finished_;
        size_t  leading_crlf_;
		size_t	total_header_bytes_;
	
	public:
		static HTTPMethod get_method( const std::string& );
		std::string get_method_name() const;
		HTTPRequest();
		~HTTPRequest();
		bool want_keep_alive();
		bool version_supported() const;
		void add_request_line( RequestLine const& request_line );
		void add_request_header( const std::pair<std::string, std::string>& header );
		bool finished() const;
		void reset();
		RequestState::Type current_state() const;
		size_t total_header_bytes() const;
		const HTTPRequestData& data() const;
		void on_finished();
	};
}


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
		
	};

	class HTTPRequestData {
		
	private:
		static const size_t MAX_HEADER_COUNT = 100;
		HTTPMethod method_;
		std::string version_;
		std::string unparsed_uri_;

		std::string query_string_;

		std::map<std::string, std::string> headers_;
		RequestState::Type request_state_;
		/* metadata */
		bool    finished_;
        size_t  leading_crlf_;
		size_t	total_header_bytes_;
	
	public:
		static HTTPMethod get_method( const std::string& );
		std::string get_method_name() const;
		HTTPRequestData();
		~HTTPRequestData();
		bool want_keep_alive();
		bool version_supported() const;
		void add_request_line( RequestLine const& request_line );
		void add_request_header( const std::pair<std::string, std::string>& header );
		bool finished() const;
		void reset();
		RequestState::Type current_state() const;
		size_t total_header_bytes() const;
		const std::string& get( const std::string& key ) const;
		const std::string& unparsed_uri() const;
		HTTPMethod method() const;
		std::map<std::string, std::string> const& headers() const;
		std::string const& query() const;
	};
}


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

	enum RequestState {
		REQUEST_LINE,
		HEADERS,
		BODY,
		DONE,
		ERROR
	};

	struct RequestLine {
		HTTPMethod method;
		std::string version;
		std::string uri;
		
	};

	class HTTPRequestData {
		
	private:
		HTTPMethod method_;
		std::string version_;
		std::string url_;

		std::string uri_path_;
		std::string query_;

		std::map<std::string, std::string> headers_;
		RequestState request_state_;
		/* metadata */
		bool    finished_;
        size_t  leading_crlf_;
		size_t	total_header_bytes_;
	
	public:
		static HTTPMethod get_method( std::string& s );
		static std::string get_method_name( HTTPMethod m );
		HTTPRequestData();
		~HTTPRequestData();
		bool want_keep_alive();
		bool version_supported() const;
		void add_request_line( RequestLine const& request_line );
		void add_request_header( const std::pair<std::string, std::string> header );
		bool finished();
		void reset();
		RequestState current_state() const;
		size_t total_header_bytes() const;
	};
}


#pragma once 

#include <string>
#include <map>
#include <vector>
#include "Headers.hpp"

namespace http {


	enum Method
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
		Method method;
		std::string version;
		std::string uri;
		
		RequestLine(): method(UNKNOWN), version(""), uri("") {}

		~RequestLine() {}
	};

	struct RequestData {
		Method	method;
		std::string version;
		std::string unparsed_uri;
		std::string resource_path;
		std::string query_string;
		Headers		headers;
	};

	
	class Request {
		
	private:
		static const size_t MAX_HEADER_COUNT = 100;

		RequestData data_;
	
		RequestState::Type request_state_;

		/* metadata */
		bool    finished_;
        size_t  leading_crlf_;
		size_t	total_header_bytes_;
	
	public:
		static Method get_method( const std::string& );
		std::string get_method_name() const;
		Request();
		~Request();
		bool keep_aliv();
		bool version_supported() const;
		void add_request_line( RequestLine const& request_line );
		void add_request_header( const std::pair<std::string, std::string>& header );
		bool finished() const;
		void reset();
		RequestState::Type current_state() const;
		size_t total_header_bytes() const;
		const RequestData& data() const;
		void on_finished();
	};
}


#pragma once 

#include <string>
#include <map>
#include <vector>
#include "Headers.hpp"
#include "RequestBody.hpp"

namespace http {

	enum Method {
		GET,
		POST,
		PUT,
		PATCH,
		DELETE,
		UNKNOWN
	};

	enum RequestState {
		REQUEST_LINE,
		REQ_HEADERS,
		REQ_BODY,
		REQ_DONE,
		REQ_ERROR
	};
	
	struct RequestLine {
		Method method;
		std::string version;
		std::string uri;
		RequestLine(): method(UNKNOWN), version(""), uri("") {}
		~RequestLine() {}
	};

	struct RequestData {
		Method		method;
		std::string version;
		std::string unparsed_uri;
		std::string resource_path;
		std::string query_string;
		Headers		headers;
		size_t		total_header_bytes_;
		size_t  	leading_crlf_;
	};

	
	class Request {		
	private:
		static const size_t MAX_HEADER_COUNT = 100;
		RequestData data_;
		RequestState request_state_;
		RequestBody body;
		/* metadata */

	public:
		Request();
		~Request();
		
		std::string get_method_name() const;
		bool keep_aliv();
		bool version_supported() const;
		void add_request_line( RequestLine const& request_line );
		void add_request_header( const std::pair<std::string, std::string>& header );
		RequestState current_state() const;
		size_t total_header_bytes() const;
		const RequestData& data() const;

		static Method get_method( const std::string& );
	};
}

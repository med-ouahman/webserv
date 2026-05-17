
#include "HTTPRequestData.hpp"
#include <unistd.h>
#include <iostream>

namespace http {

    HTTPMethod HTTPRequestData::get_method( std::string const& m ) {
		if (m == "GET")
			return GET;
		if (m == "POST")
			return POST;
		if (m == "PUT")
			return PUT;
		if (m == "PATCH")
			return PATCH;
		if (m == "DELETE")
			return DELETE;
		return UNKNOWN;
	}
	
	std::string HTTPRequestData::get_method_name() const {
		if (method_ == GET)
			return "GET";
		if (method_ == POST)
			return "POST";
		if (method_ == PUT)
			return "PUT";
		if (method_ == PATCH)
			return "PATCH";
		if (method_ == DELETE)
			return "DELETE";
		return "UNKNOWN";
	}
		
	HTTPRequestData::HTTPRequestData()
	: method_(UNKNOWN),
	request_state_(RequestState::REQUEST_LINE),
	finished_(false),
	leading_crlf_(0),
	total_header_bytes_(0) {}

	HTTPRequestData::~HTTPRequestData() {
		
	}

	bool HTTPRequestData::want_keep_alive() {
		if (version_ == "HTTP/1.1") {
			return headers_["connection"] != "close";
		} else if (version_ == "HTTP/1.0") {
			return headers_["connection"] == "keep-alive";
		}

		return false;
	}

	bool HTTPRequestData::version_supported() const {
		return version_ == "HTTP/1.0" || version_ == "HTTP/1.1";
	}

	void HTTPRequestData::add_request_header( const std::pair<std::string, std::string>& header ) {
		
		if (header.first.empty()) {
			finished_ = true;
			return ;
		}
		
		total_header_bytes_ += header.first.size();
		total_header_bytes_ += header.second.size();

		headers_[header.first] = header.second;

		if (headers_.size() > MAX_HEADER_COUNT) request_state_ = RequestState::REQUEST_ERROR;
	}

	void HTTPRequestData::add_request_line( const RequestLine& request_line ) {
		method_ = request_line.method;
		version_ = request_line.version;
		unparsed_uri_ = request_line.uri;
		/* split the unparsed uri and extract the quer string*/
		query_string_ = "";
	}
	
	bool HTTPRequestData::finished() const {
		return finished_;
	}

	void HTTPRequestData::reset() {
		
	}

	RequestState::Type HTTPRequestData::current_state() const {
		return request_state_;
	}

	size_t HTTPRequestData::total_header_bytes() const {
		return total_header_bytes_;
	}


	std::string const & HTTPRequestData::get( const std::string& key ) const {
		try {
			return headers_.at(key);
		} catch(...) {
			const static std::string empty;
			return empty;
		}
	}

	std::string const& HTTPRequestData::unparsed_uri() const {
		return unparsed_uri_;
	}

	HTTPMethod HTTPRequestData::method() const {
		return method_;
	}

	std::map<std::string, std::string> const& HTTPRequestData::headers() const {
		return headers_;
	}

	std::string const& HTTPRequestData::query() const {
		return query_string_;
	}
}

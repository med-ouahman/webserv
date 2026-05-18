
#include "HTTPRequest.hpp"
#include <unistd.h>
#include <iostream>

namespace http {

    HTTPMethod HTTPRequest::get_method( std::string const& m ) {
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
	
	std::string HTTPRequest::get_method_name() const {
		if (data_.method == GET)
			return "GET";
		if (data_.method == POST)
			return "POST";
		if (data_.method == PUT)
			return "PUT";
		if (data_.method == PATCH)
			return "PATCH";
		if (data_.method == DELETE)
			return "DELETE";
		return "UNKNOWN";
	}
		
	HTTPRequest::HTTPRequest()
	: data_(),
	request_state_(RequestState::REQUEST_LINE),
	finished_(false),
	leading_crlf_(0),
	total_header_bytes_(0) {}

	HTTPRequest::~HTTPRequest() {
		
	}

	bool HTTPRequest::want_keep_alive() {
		if (data_.version == "HTTP/1.1") {
			return data_.headers["connection"] != "close";
		} else if (data_.version == "HTTP/1.0") {
			return data_.headers["connection"] == "keep-alive";
		}

		return false;
	}

	bool HTTPRequest::version_supported() const {
		return data_.version == "HTTP/1.0" || data_.version == "HTTP/1.1";
	}

	void HTTPRequest::add_request_header( const std::pair<std::string, std::string>& header ) {
		
		if (header.first.empty()) {
			finished_ = true;
			std::cout << "Somethings never change\n";
			return ;
		}
		
		total_header_bytes_ += header.first.size();
		total_header_bytes_ += header.second.size();

		data_.headers[header.first] = header.second;

		if (data_.headers.size() > MAX_HEADER_COUNT) request_state_ = RequestState::REQUEST_ERROR;
	}

	void HTTPRequest::add_request_line( const RequestLine& request_line ) {
		data_.method = request_line.method;
		data_.version = request_line.version;
		data_.unparsed_uri = request_line.uri;
		/* split the unparsed uri and extract the quer string*/
		data_.query_string = "";
		data_.resource_path = "";

		request_state_ = RequestState::HEADERS;
	}
	
	bool HTTPRequest::finished() const {
		return finished_;
	}

	void HTTPRequest::reset() {
		finished_ = false;
	}

	RequestState::Type HTTPRequest::current_state() const {
		return request_state_;
	}

	size_t HTTPRequest::total_header_bytes() const {
		return total_header_bytes_;
	}

	const HTTPRequestData& HTTPRequest::data() const {
		return data_;
	}

	void HTTPRequest::on_finished() {
		finished_ = true;
	}
}

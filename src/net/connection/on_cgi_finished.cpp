#include "Connection.hpp"
#include "CGIBodyProvider.hpp"
#include <cstdlib>
#include "EventLoop.hpp"

namespace core {

	void Connection::on_cgi_finished() {
		std::cout << "CGI is done\n";
		state = ConnectionState::CGI_FINISH;
		release_cgi_handler();
	}

	void Connection::bind_cgi() {

		http::CGIRequestHandler* h = static_cast<http::CGIRequestHandler*>(request_handler);
		const http::Headers& headers = h->cgi_headers();

		bool has_content_len = not headers.get("content-length").empty();

		char* end;

		size_t body_size = http::CGIRequestHandler::MAX_CGI_BODY_LEN;

		if (has_content_len) body_size = ::strtoul(response.headers.get("content-length").c_str(), &end, 10);

		if ((has_content_len && *end != '\0') || body_size > http::CGIRequestHandler::MAX_CGI_BODY_LEN) {
			state = ConnectionState::WRITING;
			response.status_code = http::BAD_GATEWAY;
			close_after_write = true;
			return ;
		}

		http::BodySendMethod::Type body_method = has_content_len
			? http::BodySendMethod::CONTENT_LENGTH: http::BodySendMethod::CHUNKED;

		response.body_provider = new http::CGIBodyProvider(*static_cast<http::CGIRequestHandler*>(request_handler),
			body_method, body_size);

		state = ConnectionState::WRITING;
		std::cout << "CGI BODY TYPE: " << (has_content_len?"CONTENT-LENGTH\n":"CHUNKED\n");
	}

	void Connection::on_cgi_error(http::StatusCode c ) {
		response.status_code = c;
		state = ConnectionState::WRITING;
		close_after_write = true;
	}

	void Connection::on_cgi_output_ready() {
		state = ConnectionState::WRITING;
	}

}

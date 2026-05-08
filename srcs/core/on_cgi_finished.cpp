#include "Connection.hpp"
#include "CGIBodyProvider.hpp"
#include <cstdlib>
#include "EventLoop.hpp"

namespace core {

	void Connection::on_cgi_finished() {
		std::cout << "CGI is done\n";
		state = ConnectionState::CGI_FINISH;
		cgi_detach();
	}

	void Connection::on_cgi_output_ready() {

		bool has_content_len = response.headers["content-length"].size() > 0;

		char* end;

		size_t body_size = http::CGIHandler::MAX_CGI_BODY_LEN;

		if (has_content_len) body_size = ::strtoul(response.headers["content-length"].c_str(), &end, 10);

		if ((has_content_len && *end != '\0') || body_size > http::CGIHandler::MAX_CGI_BODY_LEN) {
			state = ConnectionState::WRITING;
			response.status_code = http::BAD_GATEWAY;
			response.reason = "Bad Gateway";
			close_after_write = true;
			return ;
		}

		http::BodySendMethod::Type body_method = has_content_len
			? http::BodySendMethod::CONTENT_LENGTH: http::BodySendMethod::CHUNKED;

		response.body_provider = new http::CGIBodyProvider(*cgi_handler, body_method, body_size);
		state = ConnectionState::WRITING;
		std::cout << "CGI BODY TYPE: " << (has_content_len?"CONTENT-LENGTH\n":"CHUNKED\n");
		resume_task = true;
	}

	void Connection::on_cgi_error(http::HTTPStatusCode c, std::string const& reason ) {
		response.status_code = c;
		response.reason = reason;
		state = ConnectionState::WRITING;
		close_after_write = true;
	}

}

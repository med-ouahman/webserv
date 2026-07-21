#include "http/Request.hpp"

namespace http {

Request::Request()
	: url(),
	  path(),
	  query(),
	  headers(),
	  host(),
	  content_length(),
	  body(),
	  method(UNKNOWN),
	  version(HTTP_UNKNOWN),
	  connection(CONNECTION_DEFAULT),
	  chunked(false) {}

void Request::reset() {
	url.clear();
	path.clear();
	query = base::Optional<std::string>();
	headers.clear();
	host = base::Optional<std::string>();
	content_length = base::Optional<usize>();
	body.reset();
	method = UNKNOWN;
	version = HTTP_UNKNOWN;
	connection = CONNECTION_DEFAULT;
	chunked = false;
}

}

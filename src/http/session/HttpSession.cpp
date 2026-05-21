#include "HttpSession.hpp"


namespace http {

	HttpSession::HttpSession() {

    }

    HttpSession::~HttpSession() {

    }

    void HttpSession::consume( DataView& data_view ) {

    }

    void HttpSession::produce( BufferWriter& writer ) {
        parser.parse_http_request();
    }

    void HttpSession::reset() {

    }

    bool HttpSession::should_keep_alive() {

    }
}


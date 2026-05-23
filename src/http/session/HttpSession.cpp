#include "HttpSession.hpp"


namespace http {

	HttpSession::HttpSession():
        state(INITIAL),
        num_requests(0),
        keep_alive(false) {}

    HttpSession::~HttpSession() {

    }

    void HttpSession::consume( DataView& data_view ) {
        ScanResult r = parser.parse_http_request(data_view);
w
        if (r == SUCCESS) {

        }

        

    }

    void HttpSession::produce( BufferWriter& writer ) {
        response.produce(writer);
    }

    void HttpSession::reset() {

    }

    bool HttpSession::should_keep_alive() {

    }
}


#include "TestSupport.hpp"

#include <iostream>

namespace {

static void testPartialRequestLine(TestState& state) {
	http::Parser parser;
	http::Context context;
	const std::string request = "GET /partial";
	usize consumed;
	http::Error err = feed(parser, context, request, consumed);

	expectError(state, "partial_request_line", err, http::ERR_NONE);
	expectConsumed(state, "partial_request_line", consumed, request.size());
	expectAction(state, "partial_request_line", context.nextAction(),
		http::AC_READ);
}

static void testCompleteGetHeaders(TestState& state) {
	http::Parser parser;
	http::Context context;
	const std::string request =
		"GET /hello?x=1 HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"Connection: close\r\n"
		"\r\n";
	usize consumed;
	http::Error err = feed(parser, context, request, consumed);

	expectError(state, "complete_get_headers", err, http::ERR_NONE);
	expectConsumed(state, "complete_get_headers", consumed, request.size());
	err = feed(parser, context, "", consumed);
	expectError(state, "complete_get_headers", err, http::ERR_NONE);
	expectState(state, "complete_get_headers", context.state_,
		http::PROCESSING);
}

static void testMissingHost(TestState& state) {
	http::Parser parser;
	http::Context context;
	const std::string request =
		"GET / HTTP/1.1\r\n"
		"\r\n";
	usize consumed;
	http::Error err = feed(parser, context, request, consumed);

	expectError(state, "missing_host_request_line", err, http::ERR_NONE);
	expectConsumed(state, "missing_host", consumed, request.size());
	err = feed(parser, context, "", consumed);
	expectError(state, "missing_host", err, http::ERR_MISSING_HOST);
}

static void testDuplicateContentLength(TestState& state) {
	http::Parser parser;
	http::Context context;
	const std::string request =
		"POST /upload HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"Content-Length: 1\r\n"
		"Content-Length: 2\r\n"
		"\r\n";
	usize consumed;
	http::Error err = feed(parser, context, request, consumed);

	expectError(state, "duplicate_content_length_request_line", err,
		http::ERR_NONE);
	expectConsumed(state, "duplicate_content_length", consumed,
		request.size());
	err = feed(parser, context, "", consumed);
	expectError(state, "duplicate_content_length", err,
		http::ERR_DUPLICATE_HEADER);
}

}

int main(void) {
	TestState state;

	testPartialRequestLine(state);
	testCompleteGetHeaders(state);
	testMissingHost(state);
	testDuplicateContentLength(state);
	if (state.failures != 0)
		return 1;
	std::cout << "Parser progress tests passed" << std::endl;
	return 0;
}

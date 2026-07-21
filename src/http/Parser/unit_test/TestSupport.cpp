#include "TestSupport.hpp"

#include <iostream>

TestState::TestState()
	: failures(0) {}

void TestState::fail(const std::string& test, const std::string& message) {
	std::cerr << "FAIL: " << test << ": " << message << std::endl;
	++failures;
}

http::Error feed(http::Parser& parser, http::Context& context,
		const std::string& data, usize& consumed) {
	consumed = 0;
	return parser.progress(context, data.data(), data.size(), consumed);
}

void expectError(TestState& state, const std::string& test,
		http::Error actual, http::Error expected) {
	if (actual != expected)
		state.fail(test, "unexpected parser error");
}

void expectConsumed(TestState& state, const std::string& test,
		usize actual, usize expected) {
	if (actual != expected)
		state.fail(test, "unexpected consumed byte count");
}

void expectAction(TestState& state, const std::string& test,
		http::ContextAction actual, http::ContextAction expected) {
	if (actual != expected)
		state.fail(test, "unexpected context action");
}

void expectState(TestState& state, const std::string& test,
		http::ContextState actual, http::ContextState expected) {
	if (actual != expected)
		state.fail(test, "unexpected context state");
}

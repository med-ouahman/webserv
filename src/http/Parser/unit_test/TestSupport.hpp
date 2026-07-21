#pragma once

#include "http/Context.hpp"
#include "http/Error.hpp"
#include "http/Parser/Parser.hpp"

#include <string>

struct TestState {
	int failures;

	TestState();
	void fail(const std::string& test, const std::string& message);
};

http::Error feed(http::Parser& parser, http::Context& context,
	const std::string& data, usize& consumed);

void expectError(TestState& state, const std::string& test,
	http::Error actual, http::Error expected);
void expectConsumed(TestState& state, const std::string& test,
	usize actual, usize expected);
void expectAction(TestState& state, const std::string& test,
	http::ContextAction actual, http::ContextAction expected);
void expectState(TestState& state, const std::string& test,
	http::ContextState actual, http::ContextState expected);

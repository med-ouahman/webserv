#pragma once

#include "http/Request.hpp"

#define CRLF "\r\n"

namespace http {

enum ContextState {
	PARSING,
	PROCESSING,
	DONE,
	ERROR
};

enum ContextAction {
	AC_READ,
	AC_WRITE,
	AC_CLOSE,
	AC_NONE
};

struct Actor {
	Request request;
};

class Context {
public:
	Actor actor;
	ContextState state_;
	ContextAction action_;

	Context()
		: actor(),
		  state_(PARSING),
		  action_(AC_READ) {}

	ContextAction nextAction() const { return action_; }
};

}

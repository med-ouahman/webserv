#pragma once

#include "config/Config.hpp"
#include "http/Request.hpp"

namespace http {
namespace routing {

enum BodyPolicy {
	BODY_REJECT,
	BODY_IGNORE,
	BODY_ACCEPT
};

struct Decision {
	BodyPolicy body_policy;

	Decision();
};

Decision	route(const Request& request, const config::Config& config);

}

}

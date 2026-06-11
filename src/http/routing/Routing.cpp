#include "http/routing/Routing.hpp"

namespace http {
namespace routing {

Decision::Decision()
	: body_policy(BODY_ACCEPT) {}

Decision	route(const Request& request, const config::Config& config) {
	Decision decision;

	(void)request;
	(void)config;
	return decision;
}

}
}

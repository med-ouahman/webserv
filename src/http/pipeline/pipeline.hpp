
#pragma once

#include "base/base.hpp"
#include "http/Error.hpp"
#include "http/pipeline/RequestHandler.hpp"
#include "http/routing/Routing.hpp"

namespace http {

class Context;

base::Expected<ARequestHandler*, Error> createHandler( RequestType type, Context& context );

}


#include "http/Context.hpp"
#include "http/pipeline/pipeline.hpp"
#include "http/pipeline/handlers/DeleteHandler.hpp"
#include "http/pipeline/handlers/DirectoryHandler.hpp"
#include "http/pipeline/handlers/RedirectHandler.hpp"
#include "http/pipeline/handlers/StaticFileHandler.hpp"
#include "http/pipeline/handlers/UploadHandler.hpp"
#include "http/pipeline/handlers/CgiHandler.hpp"

#include <cstddef>
#include <new>

namespace http {

base::Expected<ARequestHandler*, Error> createHandler( RequestType type, Context& context ) {
	ARequestHandler* handler;

	handler = NULL;
	switch (type) {
		case STATIC_FILE:
			handler = new (std::nothrow) StaticFileHandler(context);
			break;
		case DIRECTORY:
			handler = new (std::nothrow) DirectoryHandler(context);
			break;
		case UPLOAD:
			handler = new (std::nothrow) UploadHandler(context);
			break;
		case CGI:
			handler = new (std::nothrow) CgiHandler(context);
			break;
		case REDIRECT:
			handler = new (std::nothrow) RedirectHandler(context);
			break;
		case DELETE_RESOURCE:
			handler = new (std::nothrow) DeleteHandler(context);
			break;
		default:
			return ERR_INTERNAL;
	}
	if (handler == NULL)
		return ERR_INTERNAL;
	return handler;
}

}


#pragma once

#include <string>
#include <map>

#include "base/base.hpp"

namespace http {

/**
	* enum Method - request method recognized by the HTTP layer
	* @UNKNOWN: parsed token is not handled by the server
	*
	* The parser reduces the request-line method token to this enum so later code
	* can branch on method semantics without repeating string comparisons.
*/
enum Method     { GET, POST, DELETE, UNKNOWN };

/**
	* enum PathType - classification of the resolved request target
	* @executable: resolved target is handled through CGI execution
	* @not_found: path resolution completed but no target exists
	*
	* This classification is produced after routing and filesystem inspection and
	* is used to select the response path.
*/
enum PathType   { file, directory, executable, not_found };

/**
	* enum Version - normalized HTTP protocol version
	* @HTTP_1_0: version with non-persistent connections unless explicitly requested
	* @HTTP_1_1: version with persistent connections unless explicitly closed
	* @HTTP_UNKNOWN: parsed version token is not supported by the server
	*
	* This value is used for protocol validation and for connection persistence
	* rules when the Connection header does not override the default.
*/
enum Version	{ HTTP_1_0, HTTP_1_1, HTTP_UNKNOWN };

struct Request {

	Method								method;
	Version								version;
	std::string							url;
	std::string							path;
	base::Optional<std::string>			query;
	std::map<std::string, std::string>	headers;

	base::Optional<usize>				content_length;
	base::io::Reader					body;

};

}

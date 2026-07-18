
#pragma once

#include <string>
#include <map>
#include <vector>

#include "base/base.hpp"

namespace http {

enum Method { GET, POST, DELETE, UNKNOWN };

inline Method methodOf(const std::string& value) {
	if (value == "GET")
		return GET;
	if (value == "POST")
		return POST;
	if (value == "DELETE")
		return DELETE;
	return UNKNOWN;
}

/**
	* This classification is produced after routing and file inspection and
	* is used to select the response path.
*/
enum PathType { file, directory, executable, not_found };

/**
	* @HTTP_1_0: version with non-persistent connections unless explicitly requested
	* @HTTP_1_1: version with persistent connections unless explicitly closed
	* @HTTP_UNKNOWN: parsed version token is not supported by the server
*/
enum Version { HTTP_1_0, HTTP_1_1, HTTP_UNKNOWN };

enum ConnectionType { CONNECTION_DEFAULT, CONNECTION_CLOSE, CONNECTION_KEEP_ALIVE };

/**
 * duplicates in HTTP are handled depending on the type of the key
 * so its better to use a vector of key and value for this, then
 * handle duplications manually
 *
 *	Methods that must not be duplicated:
 *		Host
 *		Content-Length
 *		Content-Type
 *		Transfer-Encoding
 *		Authorization
 *		Proxy-Authorization
 */
struct Header {
	std::string key;
	std::string value;
};

struct Request {

	std::string					url;
	std::string					path;
	base::Optional<std::string>	query;
	std::vector<Header>			headers;

	base::Optional<std::string>	host;
	base::Optional<usize>		content_length;
	base::io::Reader			body;

	Method						method;
	Version						version;
	ConnectionType				connection;
	bool						chunked;

	Request();
	void reset();

};

}

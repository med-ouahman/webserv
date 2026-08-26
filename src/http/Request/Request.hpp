
#pragma once

#include <string>
#include <map>
#include <vector>

#include "base/base.hpp"

namespace http {

class SessionManager;

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

struct Header { std::string key; std::string value; };

struct Request {

	std::string url;
	std::string path;
	std::vector<Header> headers;

	base::Optional<std::string> query;
	base::Optional<std::string> host;
	base::Optional<usize> content_length;

	base::io::Reader body;

	Method method;
	Version version;
	ConnectionType connection;


	/* session */
	bool sessionsEnabled;
	SessionManager* session;
	std::string currentSessionID;
	bool currentSessionValid;

	bool chunked;
	bool has_body;

	Request();
	void reset();

};

}

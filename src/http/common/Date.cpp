#include "Date.hpp"

namespace http {

std::string formatHttpDate(time_t value) {
	char buffer[64];
	struct tm* time_info = gmtime(&value);

	if (time_info == NULL)
		return "";
	if (strftime(buffer, sizeof(buffer),
		"%a, %d %b %Y %H:%M:%S GMT", time_info) == 0)
		return "";
	return buffer;
}

}

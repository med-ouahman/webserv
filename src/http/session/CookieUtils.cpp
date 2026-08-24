#include "CookieUtils.hpp"

namespace http
{

std::string extract_cookie_value(const std::map<std::string, std::string>& headers,
                                  const std::string& cookie_name)
{
	// Assumes header names have already been normalized to lowercase
	// by the parser, per project convention.
	std::map<std::string, std::string>::const_iterator it = headers.find("cookie");
	if (it == headers.end())
		return "";

	const std::string& cookies = it->second;
	std::string target = cookie_name + "=";

	std::size_t pos = 0;
	while (pos < cookies.size())
	{
		// Skip leading spaces after ';' separators
		while (pos < cookies.size() && cookies[pos] == ' ')
			++pos;

		if (cookies.compare(pos, target.size(), target) == 0)
		{
			std::size_t start = pos + target.size();
			std::size_t end = cookies.find(';', start);

			if (end == std::string::npos)
				return cookies.substr(start);
			return cookies.substr(start, end - start);
		}

		pos = cookies.find(';', pos);
		if (pos == std::string::npos)
			break;
		++pos; // skip ';'
	}

	return "";
}

} // namespace http

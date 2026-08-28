#include "Path.hpp"

namespace http {

std::string pathJoin(const std::string& left, const std::string& right) {
	if (left.empty())
		return right;
	if (right.empty())
		return left;
	std::string suffix = right[0] == '/' ? right.substr(1) : right;
	if (left == "/")
		return "/" + suffix;
	if (left[left.size() - 1] == '/')
		return left + suffix;
	return left + "/" + suffix;
}

}

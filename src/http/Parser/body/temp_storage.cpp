#include "http/Parser/body/temp_storage.hpp"

#include <cerrno>
#include <sstream>
#include <sys/stat.h>

namespace http {
namespace parser {

static std::string pathJoin(const std::string& root,
		const std::string& path) {
	if (root.empty())
		return path;
	if (root[root.size() - 1] == '/')
		return root + path;
	return root + "/" + path;
}

bool prepareTempStorage(const std::string& root) {
	std::string path = pathJoin(root, HTTP_TMP_DIR);
	struct stat info;

	if (mkdir(path.c_str(), 0700) == 0)
		return true;
	if (errno != EEXIST || stat(path.c_str(), &info) != 0)
		return false;
	return S_ISDIR(info.st_mode);
}

std::string tempBodyPath(const std::string& root, usize conn_id,
		usize request_id) {
	std::ostringstream path;

	path << pathJoin(root, HTTP_TMP_DIR) << "/body_"
		<< conn_id << "_" << request_id << ".tmp";
	return path.str();
}

}
}

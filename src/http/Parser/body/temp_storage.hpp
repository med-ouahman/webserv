#pragma once

#include "base/types.hpp"

#include <string>

#define HTTP_TMP_DIR ".tmp"

namespace http {
namespace parser {

bool prepareTempStorage(const std::string& root);
std::string tempBodyPath(const std::string& root, usize conn_id,
	usize request_id);

}
}

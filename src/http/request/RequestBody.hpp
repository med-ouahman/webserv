#pragma once

#include <string>

namespace http {
	struct RequestBody {

		enum Storage {
			MEMORY,
			FILE_TEMP,
			FILE_PERM
		};

		Storage storage;
		std::string memory;

		std::string filename;
	};

}

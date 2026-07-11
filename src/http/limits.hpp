
#pragma once

namespace http {

namespace limits {

enum {
	REQUEST_LINE_MAX_SIZE = 1024 * 8,
	HEADER_MAX_SIZE = 1024 * 32,
	BODY_MAX_SIZE = 1024 * 1024,
	CHUNK_SIZE_LINE_MAX = 1024,
	BODY_BUFFER_SIZE = 1024 * 16,
	BODY_PROGRESS_MIN_BYTES = 4096
};

}

}

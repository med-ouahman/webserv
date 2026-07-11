#pragma once

namespace http {
namespace timeout {

enum {
	REQUEST_LINE_SECONDS = 5,
	HEADER_SECONDS = 10,
	BODY_PROGRESS_SECONDS = 30,
	BODY_MIN_WAIT_SECONDS = 5,
	BODY_MIN_BYTES_PER_SECOND = 200
};

}
}

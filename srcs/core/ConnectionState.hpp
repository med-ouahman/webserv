
#pragma once

namespace core {
	enum ConnectionState {
		ACCEPTED,
		READING,
		PARSING,
		PROCESSING,
		READY_TO_WRITE,
		WRITING,
		WRITE_COMPLETE,
		ERROR,
		CLOSING,
	};
}

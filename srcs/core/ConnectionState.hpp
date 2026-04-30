
#pragma once

namespace core {
	class ConnectionState {
		public:
			enum Type {
				IDLE,
				READING,
				PROCESSING,
				WRITING,
				ERROR,
				CLOSING,
			};
	};
}

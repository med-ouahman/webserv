
#pragma once

namespace core {
	class ConnectionState {
		public:
			enum Type {
				IDLE,
				READING,
				PROCESSING,
				CGI,
				WRITING,
				ERROR,
				CLOSING,
			};
	};
}

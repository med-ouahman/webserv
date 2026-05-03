
#pragma once

namespace core {
	class ConnectionState {
		public:
			enum Type {
				IDLE,
				READING,
				PROCESSING,
				CGI,
				CGI_FINISH,
				WRITING,
				ERROR,
				CLOSING,
			};
	};
}

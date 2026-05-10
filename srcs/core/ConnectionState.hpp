
#pragma once

namespace core {

	class RequestPhase {
		public:
			enum Type {
				BUILDING,
				RESOLVING,
				PROCESSING
			};
	};

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

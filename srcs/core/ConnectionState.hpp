
#pragma once

namespace core {

	class RequestPhase {
		public:
			enum Type {
				BUILDING,
				RESOLVING,
				READING_BODY,
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


#pragma once

namespace core {

	class RequestPhase {
		public:
			enum Type {
				INITIAL,
				BUILDING,
				RESOLVING,
				READING_BODY,
				PROCESSING,
				WRITING_RESPONSE,
				FINAL,
				IDLE,
				ERROR
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

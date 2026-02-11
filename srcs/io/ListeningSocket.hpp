#pragma once

namespace io {

	class ListeningSocket {
		private:
			int fd;
			ListeningSocket( const ListeningSocket& ) = delete;
			ListeningSocket& operator=( const ListeningSocket& ) = delete;
		public:
			int fd() { return fd; }
			ListeningSocket() {
				fd = -1;
			}
	};
}

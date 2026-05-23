#pragma once

#include <unistd.h>

class Fd {

	private:
		int fd_;
	public:
		
		Fd( int fd ): fd_(fd) {}
		
		~Fd() {
			if (fd_ >= 0) ::close(fd_);
			fd_ = -1;
		}

		int fd() const {
			return fd_;
		}
};

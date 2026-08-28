#include "Writer.hpp"
#include <algorithm>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

namespace base {
namespace io {

void Writer::close_fd() {
	if (fd_ >= 0)
		::close(fd_);
	fd_ = -1;
}

bool Writer::open_file() {
	if (fd_ >= 0)
		return true;
	if (path_.empty())
		return false;
	fd_ = ::open(path_.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);
	return fd_ >= 0;
}

Writer::Writer()
	: path_(), buffer_(NULL), capacity_(0), used_(0), fd_(-1),
	  type_(NONE) {}

Writer::Writer(const std::string& path, char* buffer, usize capacity)
	: path_(), buffer_(NULL), capacity_(0), used_(0), fd_(-1),
	  type_(NONE) {
	reset(path, buffer, capacity);
}

Writer::~Writer() {
	close_fd();
}

bool Writer::reset() {
	if (type_ == BUFFERED_FILE) {
		close_fd();
		used_ = 0;
		return true;
	}
	close_fd();
	type_ = NONE;
	buffer_ = NULL;
	capacity_ = 0;
	used_ = 0;
	return true;
}

bool Writer::reset(const std::string& path, char* buffer, usize capacity) {
	close_fd();
	type_ = BUFFERED_FILE;
	path_ = path;
	buffer_ = buffer;
	capacity_ = capacity;
	used_ = 0;
	return !path_.empty() && buffer_ != NULL && capacity_ > 0;
}

base::Expected<usize, Error> Writer::write(const char* data, usize size) {
	if (size == 0)
		return base::Expected<usize, Error>(static_cast<usize>(0));
	if (type_ == BUFFERED_FILE) {
		usize written = 0;
		while (written < size) {
			usize available = used_ < capacity_ ? capacity_ - used_ : 0;
			if (available == 0) {
				base::Expected<usize, Error> flushed = flush();
				if (!flushed)
					return flushed;
				available = capacity_;
			}
			usize n = std::min(size - written, available);
			if (n > 0)
				::memcpy(buffer_ + used_, data + written, n);
			used_ += n;
			written += n;
			if (used_ == capacity_) {
				base::Expected<usize, Error> flushed = flush();
				if (!flushed)
					return flushed;
			}
		}
		return base::Expected<usize, Error>(written);
	}
	return base::Expected<usize, Error>(OPEN_FAILED);
}

base::Expected<usize, Error> Writer::flush() {
	usize total;
	ssize_t n;

	if (type_ != BUFFERED_FILE)
		return base::Expected<usize, Error>(static_cast<usize>(0));
	if (used_ == 0)
		return base::Expected<usize, Error>(static_cast<usize>(0));
	if (!open_file())
		return base::Expected<usize, Error>(OPEN_FAILED);
	total = 0;
	while (total < used_) {
		n = ::write(fd_, buffer_ + total, used_ - total);
		if (n < 0 && errno == EINTR)
			continue;
		if (n <= 0) {
			if (total > 0) {
				::memmove(buffer_, buffer_ + total, used_ - total);
				used_ -= total;
			}
			return base::Expected<usize, Error>(WRITE_FAILED);
		}
		total += static_cast<usize>(n);
	}
	used_ = 0;
	return base::Expected<usize, Error>(total);
}

char* Writer::data() {
	if (type_ != BUFFERED_FILE || used_ == 0)
		return NULL;
	return buffer_;
}

usize Writer::size() const {
	return used_;
}

bool Writer::file_created() const {
	return fd_ >= 0;
}

const std::string& Writer::path() const {
	return path_;
}

}

}

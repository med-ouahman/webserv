#include "Reader.hpp"
#include <algorithm>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

namespace base {
namespace io {

void Reader::close_fd() {
	if (fd_ >= 0 && owns_fd_)
		::close(fd_);
	fd_ = -1;
	owns_fd_ = false;
}

Reader::Reader()
	: type_(NONE), path_(), fd_(-1), buffer_(NULL), size_(0), offset_(0),
	  owns_fd_(false) {}

Reader::Reader(const std::string& path)
	: type_(NONE), path_(), fd_(-1), buffer_(NULL), size_(0), offset_(0),
	  owns_fd_(false) {
	reset(path);
}

Reader::Reader(i32 fd, bool owns_fd)
	: type_(NONE), path_(), fd_(-1), buffer_(NULL), size_(0), offset_(0),
	  owns_fd_(false) {
	reset(fd, owns_fd);
}

Reader::Reader(const char* buffer, usize size)
	: type_(NONE), path_(), fd_(-1), buffer_(NULL), size_(0), offset_(0),
	  owns_fd_(false) {
	reset(buffer, size);
}

Reader::~Reader() {
	close_fd();
}

bool Reader::reset() {
	if (type_ == FILE && !path_.empty())
		return reset(path_);
	close_fd();
	type_ = NONE;
	buffer_ = NULL;
	size_ = 0;
	offset_ = 0;
	return true;
}

bool Reader::reset(const std::string& path) {
	close_fd();
	type_ = FILE;
	path_ = path;
	buffer_ = NULL;
	size_ = 0;
	offset_ = 0;
	if (path_.empty())
		return true;
	fd_ = ::open(path_.c_str(), O_RDONLY);
	owns_fd_ = true;
	return fd_ >= 0;
}

bool Reader::reset(i32 fd, bool owns_fd) {
	close_fd();
	type_ = FILE;
	path_.clear();
	fd_ = fd;
	owns_fd_ = owns_fd;
	buffer_ = NULL;
	size_ = 0;
	offset_ = 0;
	return fd_ >= 0;
}

bool Reader::reset(const char* buffer, usize size) {
	close_fd();
	type_ = BUFFER;
	path_.clear();
	buffer_ = buffer;
	size_ = size;
	offset_ = 0;
	return buffer_ != NULL || size_ == 0;
}

base::Expected<usize, Error> Reader::read(char* buff, usize max_size) {
	if (max_size == 0)
		return base::Expected<usize, Error>(static_cast<usize>(0));
	if (type_ == BUFFER) {
		usize available = offset_ < size_ ? size_ - offset_ : 0;
		usize n = std::min(max_size, available);
		if (n > 0)
			::memcpy(buff, buffer_ + offset_, n);
		offset_ += n;
		return base::Expected<usize, Error>(n);
	}
	if (type_ != FILE || fd_ < 0)
		return base::Expected<usize, Error>(OPEN_FAILED);
	ssize_t n = ::read(fd_, buff, max_size);
	if (n < 0)
		return base::Expected<usize, Error>(READ_FAILED);
	return base::Expected<usize, Error>(static_cast<usize>(n));
}

Reader::Type Reader::type() const {
	return type_;
}

}

}

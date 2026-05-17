#include "Writer.hpp"
#include <algorithm>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

namespace Base {
namespace io {

void Writer::close_fd() {
	if (fd_ >= 0 && owns_fd_)
		::close(fd_);
	fd_ = -1;
	owns_fd_ = false;
}

Writer::Writer()
	: path_(), buffer_(NULL), capacity_(0), used_(0), offset_(0), fd_(-1),
	  type_(NONE), owns_fd_(false) {}

Writer::Writer(const std::string& path)
	: path_(), buffer_(NULL), capacity_(0), used_(0), offset_(0), fd_(-1),
	  type_(NONE), owns_fd_(false) {
	reset(path);
}

Writer::Writer(i32 fd, bool owns_fd)
	: path_(), buffer_(NULL), capacity_(0), used_(0), offset_(0), fd_(-1),
	  type_(NONE), owns_fd_(false) {
	reset(fd, owns_fd);
}

Writer::Writer(char* buffer, usize capacity)
	: path_(), buffer_(NULL), capacity_(0), used_(0), offset_(0), fd_(-1),
	  type_(NONE), owns_fd_(false) {
	reset(buffer, capacity);
}

Writer::~Writer() {
	close_fd();
}

bool Writer::reset() {
	if (type_ == FILE && !path_.empty())
		return reset(path_);
	if (type_ == BUFFER) {
		used_ = 0;
		offset_ = 0;
		return true;
	}
	close_fd();
	type_ = NONE;
	buffer_ = NULL;
	capacity_ = 0;
	used_ = 0;
	offset_ = 0;
	return true;
}

bool Writer::reset(const std::string& path) {
	close_fd();
	type_ = FILE;
	path_ = path;
	buffer_ = NULL;
	capacity_ = 0;
	used_ = 0;
	offset_ = 0;
	if (path_.empty())
		return true;
	fd_ = ::open(path_.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);
	owns_fd_ = true;
	return fd_ >= 0;
}

bool Writer::reset(i32 fd, bool owns_fd) {
	close_fd();
	type_ = FILE;
	path_.clear();
	fd_ = fd;
	owns_fd_ = owns_fd;
	buffer_ = NULL;
	capacity_ = 0;
	used_ = 0;
	offset_ = 0;
	return fd_ >= 0;
}

bool Writer::reset(char* buffer, usize capacity) {
	close_fd();
	type_ = BUFFER;
	path_.clear();
	buffer_ = buffer;
	capacity_ = capacity;
	used_ = 0;
	offset_ = 0;
	return buffer_ != NULL || capacity_ == 0;
}

Base::Expected<usize, Error> Writer::write(const u8* data, usize size) {
	if (size == 0)
		return Base::Expected<usize, Error>(static_cast<usize>(0));
	if (type_ == BUFFER) {
		usize available = used_ < capacity_ ? capacity_ - used_ : 0;
		usize n = std::min(size, available);
		if (n > 0)
			::memcpy(buffer_ + used_, data, n);
		used_ += n;
		return Base::Expected<usize, Error>(n);
	}
	if (type_ != FILE || fd_ < 0)
		return Base::Expected<usize, Error>(OPEN_FAILED);
	ssize_t n = ::write(fd_, data, size);
	if (n < 0)
			return Base::Expected<usize, Error>(WRITE_FAILED);
	return Base::Expected<usize, Error>(static_cast<usize>(n));
}

Base::Expected<usize, Error> Writer::write(const char* data, usize size) {
	if (size == 0)
		return Base::Expected<usize, Error>(static_cast<usize>(0));
	if (type_ == BUFFER) {
		usize available = used_ < capacity_ ? capacity_ - used_ : 0;
		usize n = std::min(size, available);
		if (n > 0)
			::memcpy(buffer_ + used_, data, n);
		used_ += n;
		return Base::Expected<usize, Error>(n);
	}
	if (type_ != FILE || fd_ < 0)
		return Base::Expected<usize, Error>(OPEN_FAILED);
	ssize_t n = ::write(fd_, data, size);
	if (n < 0)
		return Base::Expected<usize, Error>(WRITE_FAILED);
	return Base::Expected<usize, Error>(static_cast<usize>(n));
}

Base::Expected<usize, Error> Writer::write(const std::string& data) {
	return write(data.data(), data.size());
}

char* Writer::data() {
	if (type_ != BUFFER || offset_ >= used_)
		return NULL;
	return buffer_ + offset_;
}

usize Writer::size() const {
	return used_;
}

usize Writer::offset() const {
	return offset_;
}

usize Writer::remaining() const {
	if (type_ != BUFFER || offset_ >= used_)
		return 0;
	return used_ - offset_;
}

void Writer::advance(usize size) {
	usize left = remaining();
	offset_ += size < left ? size : left;
}

Writer::Type Writer::type() const {
	return type_;
}

}

}

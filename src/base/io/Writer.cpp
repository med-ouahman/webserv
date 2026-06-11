#include "Writer.hpp"
#include <algorithm>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

namespace base {
namespace io {

void Writer::close_fd() {
	if (fd_ >= 0 && owns_fd_)
		::close(fd_);
	fd_ = -1;
	owns_fd_ = false;
}

bool Writer::open_file() {
	if (fd_ >= 0)
		return true;
	if (path_.empty())
		return false;
	fd_ = ::open(path_.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);
	owns_fd_ = true;
	return fd_ >= 0;
}

Writer::Writer()
	: path_(), buffer_(NULL), capacity_(0), used_(0), offset_(0), fd_(-1),
	  type_(NONE), owns_fd_(false) {}

Writer::Writer(const std::string& path, char* buffer, usize capacity)
	: path_(), buffer_(NULL), capacity_(0), used_(0), offset_(0), fd_(-1),
	  type_(NONE), owns_fd_(false) {
	reset(path, buffer, capacity);
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
	if (type_ == BUFFER || type_ == BUFFERED_FILE) {
		close_fd();
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

bool Writer::reset(const std::string& path, char* buffer, usize capacity) {
	close_fd();
	type_ = BUFFERED_FILE;
	path_ = path;
	buffer_ = buffer;
	capacity_ = capacity;
	used_ = 0;
	offset_ = 0;
	return !path_.empty() && buffer_ != NULL && capacity_ > 0;
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

base::Expected<usize, Error> Writer::write(const char* data, usize size) {
	if (size == 0)
		return base::Expected<usize, Error>(static_cast<usize>(0));
	if (type_ == BUFFER) {
		usize available = used_ < capacity_ ? capacity_ - used_ : 0;
		if (size > available)
			return base::Expected<usize, Error>(NO_SPACE);
		if (size > 0)
			::memcpy(buffer_ + used_, data, size);
		used_ += size;
		return base::Expected<usize, Error>(size);
	}
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

base::Expected<usize, Error> Writer::write(const std::string& data) {
	return write(data.data(), data.size());
}

base::Expected<usize, Error> Writer::flush() {
	if (type_ != BUFFERED_FILE)
		return base::Expected<usize, Error>(static_cast<usize>(0));
	if (used_ == 0)
		return base::Expected<usize, Error>(static_cast<usize>(0));
	if (!open_file())
		return base::Expected<usize, Error>(OPEN_FAILED);
	ssize_t n = ::write(fd_, buffer_, used_);
	if (n < 0)
		return base::Expected<usize, Error>(WRITE_FAILED);
	used_ = 0;
	offset_ = 0;
	return base::Expected<usize, Error>(static_cast<usize>(n));
}

char* Writer::data() {
	if ((type_ != BUFFER && type_ != BUFFERED_FILE) || offset_ >= used_)
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
	if ((type_ != BUFFER && type_ != BUFFERED_FILE) || offset_ >= used_)
		return 0;
	return used_ - offset_;
}

void Writer::advance(usize size) {
	usize left = remaining();
	offset_ += size < left ? size : left;
}

bool Writer::file_created() const {
	return fd_ >= 0;
}

const std::string& Writer::path() const {
	return path_;
}

Writer::Type Writer::type() const {
	return type_;
}

}

}

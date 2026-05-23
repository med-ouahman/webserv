#pragma once

#include <string>
#include "base/types.hpp"
#include "base/Expected.hpp"
#include "base/io/Error.hpp"

namespace base {
namespace io {

class Writer {
public:
	enum Type {
		NONE,
		FILE,
		BUFFER
	};

private:
	std::string path_;

	char* buffer_;
	usize capacity_;
	usize used_;
	usize offset_;

	i32 fd_;
	Type type_;
	bool owns_fd_;

	Writer(const Writer&);
	Writer& operator=(const Writer&);

	void close_fd();

public:

	Writer();

	explicit Writer(const std::string& path);
		Writer(i32 fd, bool owns_fd);

		Writer(char* buffer, usize capacity);

	/**
	 * ~Writer - release owned resources
	 *
	 * Closes fd_ only when owns_fd_ is true.
	 */
	~Writer();

	bool reset();

	bool reset(const std::string& path);
	bool reset(i32 fd, bool owns_fd);
		
	bool reset(char* buffer, usize capacity);

	base::Expected<usize, Error> write(const u8* data, usize size);
	base::Expected<usize, Error> write(const char* data, usize size);
	base::Expected<usize, Error> write(const std::string& data);

	char* data();
	usize size() const;
	usize offset() const;
	usize remaining() const;
	void advance(usize size);

	Type type() const;
};

}

}

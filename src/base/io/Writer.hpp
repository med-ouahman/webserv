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
		BUFFER,
		BUFFERED_FILE
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
	bool open_file();

public:

	Writer();

	Writer(const std::string& path, char* buffer, usize capacity);

	Writer(char* buffer, usize capacity);

	/**
	 * ~Writer - release owned resources
	 *
	 * Closes fd_ only when owns_fd_ is true.
	 */
	~Writer();

	bool reset();

	bool reset(const std::string& path, char* buffer, usize capacity);
		
	bool reset(char* buffer, usize capacity);

	base::Expected<usize, Error> write(const char* data, usize size);
	base::Expected<usize, Error> write(const std::string& data);
	base::Expected<usize, Error> flush();

	char* data();
	usize size() const;
	usize offset() const;
	usize remaining() const;
	void advance(usize size);

	bool file_created() const;
	const std::string& path() const;
	Type type() const;
};

}

}

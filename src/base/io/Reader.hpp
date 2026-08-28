
#pragma once

#include <string>
#include "base/types.hpp"
#include "base/Expected.hpp"
#include "base/io/Error.hpp"

namespace base {
namespace io {

class Reader {
public:
	enum Type {
		NONE,
		FILE,
		BUFFER
	};

private:
	Type type_;
	std::string path_;
	i32 fd_;
	const char* buffer_;
	usize size_;
	usize offset_;
	bool owns_fd_;

	/* Disable copy */
	Reader(const Reader&);
	Reader& operator=(const Reader&);

	void close_fd();

public:

	Reader();
	~Reader();

	bool reset();
	bool reset(const std::string& path);
	bool reset(const char* buffer, usize size);

	base::Expected<usize, Error> read(char* buff, usize max_size);

	Type type() const;
	const std::string& path() const;
	const char* data() const;
	usize size() const;
};

}

}

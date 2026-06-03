
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
	const u8* buffer_;
	usize size_;
	usize offset_;
	bool owns_fd_;

	/* Disable copy */
	Reader(const Reader&);
	Reader& operator=(const Reader&);

	void close_fd();

public:

	Reader();
	explicit Reader(const std::string& path);
	Reader(i32 fd, bool owns_fd);
	Reader(const u8* buffer, usize size);
	~Reader();

	bool reset();
	bool reset(const std::string& path);
	bool reset(i32 fd, bool owns_fd);
	bool reset(const u8* buffer, usize size);

	base::Expected<usize, Error> read(u8* buff, usize max_size);
	base::Expected<usize, Error> read(char* buff, usize max_size);

	Type type() const;
};

}

}

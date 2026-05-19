
#pragma once

#include <string>
#include "utils/baselib/types.hpp"
#include "utils/baselib/Expected.hpp"
#include "utils/baselib/io/Error.hpp"

namespace Base {
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

	Base::Expected<usize, Error> read(u8* buff, usize max_size);
	Base::Expected<usize, Error> read(char* buff, usize max_size);

	Type type() const;
};

}

}

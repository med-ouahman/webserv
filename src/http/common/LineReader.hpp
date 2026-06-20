#pragma once

#include <string>
#include "BufferReader.hpp"

namespace http {

enum ReadResult {
	SUCCESS,
	NEED_MORE,
	LIMIT_EXCEEDED,
};

class LineReader {
private:
	std::string line_;
	bool		cr_found;
public:
	void reset();
	std::string const& line();
	ReadResult readline(BufferReader& buf, size_t max_block_len);
	
	LineReader();
	~LineReader();
};

}

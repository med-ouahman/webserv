#pragma once

#include <string>
#include "BufferReader.hpp"

namespace http {

enum ScanResult {
	SUCCESS,
	NEED_MORE,
	LIMIT_EXCEEDED,
};

class LineReader {
private:
	std::string linebuff;
	bool		cr_found;
public:
	void reset();
	std::string const& line();
	ScanResult scan(BufferReader& view, size_t max_block_len);
	
	LineReader();
	~LineReader();
};

}

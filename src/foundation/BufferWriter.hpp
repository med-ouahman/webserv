#pragma once

#include <stddef.h>
#include <string>

class BufferWriter {

private:
	std::string storage_;
	size_t used_;
	size_t offset_;
	BufferWriter(const BufferWriter& other);
	BufferWriter& operator=(const BufferWriter& other);

public:
	BufferWriter(size_t capacity);
	~BufferWriter();
	char* write_ptr();
	const char* base() const;
	const char* read_ptr() const;
	size_t length();
	bool full();
	bool empty();
	size_t bytes_pending();
	void advance_read(size_t __n);
	void advance_write(size_t __n);
	void reset();
	size_t capacity();
	size_t bytes_free();
	size_t write(const char* src, size_t n__);
};


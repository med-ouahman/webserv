#pragma once

#include <stddef.h>
#include <string>

class BufferWriter {
private:
	const static std::size_t BuffSize = 4096;

	char		storage_[BuffSize];
	size_t		capacity_;
	size_t		used_;
	size_t		r_offset_;

	BufferWriter(const BufferWriter& other);
	BufferWriter& operator=(const BufferWriter& other);

public:
	BufferWriter();
	~BufferWriter();

	char* write_ptr();
	
	const char* read_ptr() const;
	
	bool full();
	bool empty();
	
	void advance_read(size_t __n);
	void advance_write(size_t __n);
	void reset();
	void pop(size_t n__);
	
	size_t bytes_pending();
	size_t capacity();
	size_t bytes_free();
	size_t write(const char* src, size_t n__);
	size_t size();
};


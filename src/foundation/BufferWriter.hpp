#pragma once

#include <stddef.h>
#include <string>

class BufferWriter {
private:
	std::string	storage_;
	size_t		used_;
	size_t		offset_;

	BufferWriter(const BufferWriter& other);
	BufferWriter& operator=(const BufferWriter& other);

public:
	BufferWriter(size_t capacity);
	~BufferWriter();

	char* write_ptr();
	
	const char* base() const;
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
	size_t length();
};


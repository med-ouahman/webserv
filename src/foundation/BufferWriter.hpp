#pragma once

#include <stddef.h>

class BufferWriter {

	private:
		const static size_t BUFFER_SIZE = 1024 * 4;
		char buff_[BUFFER_SIZE];
		const size_t capacity_ = BUFFER_SIZE;
		size_t offset_;
		size_t size_;
		BufferWriter( const BufferWriter& other );
		BufferWriter& operator=( const BufferWriter& other );
	
	public:
		BufferWriter();
		~BufferWriter();
		char* data();
		char* write_ptr();
		size_t offset();
		size_t size();
		bool full();
		bool empty();
		size_t remaining();
		void advance( size_t n );
		void reset();
		size_t capacity();
		size_t bytes_free();
		size_t write( const char* src, size_t n__ );
};


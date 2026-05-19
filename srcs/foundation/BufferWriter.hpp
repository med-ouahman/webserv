#pragma once

#include <iostream>


class BufferWriter {

	private:
		char* buff_;
		size_t capacity_;
		size_t offset_;
		size_t size_;
		BufferWriter( const BufferWriter& other );
		BufferWriter& operator=( const BufferWriter& other );
	
	public:
		explicit BufferWriter( char* b, size_t cap );
		~BufferWriter();
		void update( char* buff, size_t size );
		void update( size_t n );
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


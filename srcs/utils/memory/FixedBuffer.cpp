#include "FixedBuffer.hpp"

namespace mem {

FixedBufferAllocator::FixedBufferAllocator( char* buffer, usize size )
	: buf(buffer), capacity(size), used(0) {}

void* FixedBufferAllocator::alloc( usize size ) {
	usize aligned_size = (size + 7) & ~7;
	if (used + aligned_size > capacity)
		return NULL;
	void* ptr = buf + used;
	used += aligned_size;
	return ptr;
}

void FixedBufferAllocator::dealloc( void*, usize ) {}

void FixedBufferAllocator::reset() {
	used = 0;
}

}

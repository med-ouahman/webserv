#pragma once

#include "types.hpp"

namespace mem {

struct FixedBufferAllocator {
	char*	buf;
	usize	capacity;
	usize	used;

	FixedBufferAllocator( char* buffer, usize size );

	void*	alloc( usize size );
	void	dealloc( void*, usize );
	void	reset();
};

}

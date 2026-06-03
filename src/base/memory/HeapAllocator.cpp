#include "HeapAllocator.hpp"
#include <new>

namespace mem {

void* HeapAllocator::alloc( usize size ) {
	return ::operator new(size, std::nothrow);
}

void HeapAllocator::free( void* ptr ) {
	::operator delete(ptr);
}

}

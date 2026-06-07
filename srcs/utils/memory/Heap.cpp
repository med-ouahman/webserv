#include "Heap.hpp"
#include <new>

namespace mem {

void* HeapAllocator::alloc( usize size ) {
	return ::operator new(size, std::nothrow);
}

void HeapAllocator::dealloc( void* ptr, usize ) {
	::operator delete(ptr);
}

}

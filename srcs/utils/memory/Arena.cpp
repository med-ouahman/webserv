#include "Arena.hpp"

namespace mem {

ArenaAllocator::ArenaAllocator() : head(NULL), state(EMPTY) {}

ArenaAllocator::~ArenaAllocator() {
	Arena* current = head;
	while (current) {
		Arena* prev = current->prev;
		delete[] (char*)current;
		current = prev;
	}
}

char* ArenaAllocator::arena_base( Arena* arena ) {
	return (char*)(arena + 1);
}

bool ArenaAllocator::grow( usize min_size ) {
	usize capacity = head ? head->capacity + ( head->capacity / 2 ) : min_size;
	if (capacity < min_size)
		capacity = min_size;

	void* raw = new (std::nothrow) char[sizeof(Arena) + capacity];
	if (!raw) {
		state = ALLOC_FAILED;
		return false;
	}

	Arena* arena = (Arena*)raw;
	arena->prev = head;
	arena->capacity = capacity;
	arena->used = 0;
	head = arena;
	return true;
}

void* ArenaAllocator::alloc( usize size ) {
	usize aligned_size = (size + 7) & ~7;

	if (!head || head->used + aligned_size > head->capacity) {
		if (!grow(aligned_size))
			return NULL;
	}

	void* ptr = arena_base(head) + head->used;
	head->used += aligned_size;
	state = IN_USE;
	return ptr;
}

void ArenaAllocator::dealloc( void*, usize ) {}

void ArenaAllocator::reset() {
	Arena* current = head;
	while (current) {
		current->used = 0;
		current = current->prev;
	}
	state = EMPTY;
}

ArenaAllocator::State ArenaAllocator::getState() const {
	return state;
}

}

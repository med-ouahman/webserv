#pragma once

#include "types.hpp"
#include <new>

namespace mem {

class ArenaAllocator {

public:
	enum State { EMPTY, IN_USE, ALLOC_FAILED, OUT_OF_SPACE };

private:

	struct Arena {
		Arena*	prev;
		usize	capacity;
		usize	used;
	};

	Arena*	head;
	State	state;

	char*	arena_base( Arena* arena );
	bool	grow( usize min_size );

public:

	ArenaAllocator();
	~ArenaAllocator();

	void*	alloc( usize size );
	void	dealloc( void*, usize );
	void	reset();
	State	getState() const;
};

}

#pragma once

#include "../types.hpp"

namespace mem {

struct HeapAllocator {
	void*	alloc(usize size);
	void	free(void* ptr);
};

}

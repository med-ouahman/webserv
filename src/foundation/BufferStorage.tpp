#pragma once

#include <cstddef>

template <size_t N>
struct BufferStorage {
    char buff[N];
};

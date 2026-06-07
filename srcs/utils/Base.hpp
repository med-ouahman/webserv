#pragma once

#include <cstddef>
#include <stdint.h>

typedef std::size_t     usize;

typedef int8_t          i8;
typedef int16_t         i16;
typedef int32_t         i32;
typedef int64_t         i64;

typedef uint8_t         u8;
typedef uint16_t        u16;
typedef uint32_t        u32;
typedef uint64_t        u64;

typedef float           f32;
typedef double          f64;

#include "memory/memory.h"
#include "Optional.hpp"
#include "Result.hpp"
#include "Error.hpp"
#include "Logger.hpp"

#pragma once

#include <assert.h>
#include <complex.h>
#include <ctype.h>
#include <errno.h>
#include <fenv.h>
#include <float.h>
#include <inttypes.h>
#include <iso646.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stdatomic.h>
#include <stdbit.h>
#include <stdckdint.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>
#include <threads.h>
#include <time.h>
#include <uchar.h>
#include <wchar.h>
#include <wctype.h>

typedef uint8_t byte;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef size_t usize;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

#define DEBUG(message) { fprintf(stderr, "Debug: " message "\n"); }
#define DEBUGF(message, ...) { fprintf(stderr, "Debug: " message "\n", __VA_ARGS__); }

#define ERROR(message) { fprintf(stderr, "Error: " message "\n"); abort(); }
#define ERRORF(message, ...) { fprintf(stderr, "Error: " message "\n", __VA_ARGS__); abort(); }

#if defined(NDEBUG)
#define ASSERT(condition) ((void)0)
#else
#define ASSERT(condition) \
    if (!(condition)) { \
        ERRORF("%s " __FILE__ ":%d" " ASSERT failed: " #condition, __func__, __LINE__); \
    }
#endif

#define LOG(message) { fprintf(stdout, "Log: " message "\n"); }
#define LOGF(message, ...) { fprintf(stdout, "Log: " message "\n", __VA_ARGS__); }

byte *align(byte *ptr, usize alignment);
void *halloc(usize size);
void *rehalloc(byte *ptr, usize old_size, usize new_size);
void *stalloc(usize size);
void *restalloc(byte *ptr, usize old_size, usize new_size);
void destalloc(byte *ptr, usize size);
void init_heap(usize heap_size, usize stack_size);


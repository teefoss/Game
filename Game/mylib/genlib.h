// -----------------------------------------------------------------------------
// General Library
//
// General macros, integer types, I/O, and debug.
// -----------------------------------------------------------------------------
#ifndef __GENLIB_H__
#define __GENLIB_H__

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#include <SDL2/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BIT(n) (1 << n)
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#define FOR_EACH_INDEX(arr, i) for (int i = 0; i < ARRAY_SIZE(arr); i++)
#define MS2TICKS(ms, fps) ((float)(ms) / (1000.0f / (float)fps))
#define SWAP(a, b) { typeof(a) temp = a; a = b; b = tmp; }

// declare pt (something with a .x and .y) prior to use
#define LOOP_2D(pt, w, h)   for ( pt.y = 0; pt.y < h; pt.y++ ) \
                            for ( pt.x = 0; pt.x < w; pt.x++ )

#define print(var) _Generic((var),  \
        unsigned char: print_uchar, \
        int: print_int,             \
        double: print_double,       \
        float: print_float,         \
        unsigned long: print_ulong, \
        SDL_Point: print_sdl_point, \
        SDL_Rect: print_sdl_rect    \
    )(#var, var)

// Check how many ms a bit of code takes.
// Don't use this more than once per function...
#define PROFILE_START int __start_ms__ = SDL_GetTicks();
#define PROFILE_END(str) printf(str" took %d ms\n", SDL_GetTicks()-__start_ms__);

#define Error(...) { fprintf(stderr, "%s: ", __func__); \
                     fprintf(stderr, __VA_ARGS__);      \
                     fprintf(stderr, "\n");             \
                     exit(EXIT_FAILURE); }

//
// STORAGE
// Sort of experimental.
//

#define STORAGE(T, name, size) struct name { T data[size]; int count; }

// N.B. These macros only work with non-pointer STORAGE's

#define COUNT(arr)              arr.count
#define CAPACITY(arr)           (sizeof(arr.data) / sizeof(arr.data[0]))
#define CLEAR(arr)              arr.count = 0
#define REMOVE(arr, i)          arr.data[i] = arr.data[--arr.count]
#define INSERT(arr, i, value)   { arr.data[arr.count++] = arr.data[i]; \
                                arr.data[i] = value; }
#define APPEND(arr, value)      arr.data[arr.count++] = value
#define ELEMENT(arr, i)         arr.data[i]
#define LAST(arr)               arr.data[arr.count - 1]
#define FOR_EACH(it, arr)       for ( typeof(arr.data[0]) * it = arr.data; \
                                it <= &LAST(arr); \
                                it++ )

#define STACK(T, name, size)    STORAGE(T, name, size)
#define PUSH(stack, value)      APPEND(stack, value)
#define POP(stack)              stack.data[--s.count]

//
// Integer Types
//

#define U32_MAX 0xFFFFFFFF

typedef int8_t      s8;
typedef int16_t     s16;
typedef int32_t     s32;
typedef int64_t     s64;
typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;


#define PRINT_DECL(func, T) void func(const char * name, T value)
PRINT_DECL(print_uchar, unsigned char);
PRINT_DECL(print_int, int);
PRINT_DECL(print_float, float);
PRINT_DECL(print_double, double);
PRINT_DECL(print_ulong, unsigned long);
PRINT_DECL(print_sdl_point, SDL_Point);
PRINT_DECL(print_sdl_rect, SDL_Rect);

/// Open file and terminate program if it fails.
/// - Parameter mode: One of the standard `fopen()` modes,
///   e.g. `"r"` (read), `"w"` (write)
/// - Returns: The opened file.
FILE * OpenFile( const char * file_name, const char * mode );

/// The time since program start in fractional seconds.
float ProgramTime(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __GENLIB_H__ */

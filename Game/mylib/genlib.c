#include "genlib.h"

#define PRINT_DEF(func, T, format) \
    PRINT_DECL(func, T) { printf("%s: "format"\n", name, value); }

PRINT_DEF(print_uchar, unsigned char, "%d")
PRINT_DEF(print_int, int, "%d")
PRINT_DEF(print_float, float, "%f")
PRINT_DEF(print_double, double, "%f")
PRINT_DEF(print_ushort, unsigned short, "%d")
PRINT_DEF(print_ulong, unsigned long, "%lu")

void print_sdl_point(const char * name, SDL_Point point)
{
    printf("%s: (%d, %d)\n", name, point.x, point.y);
}

void print_sdl_rect(const char * name, SDL_Rect r)
{
    printf("%s: x: %d, y: %d, w: %d, h: %d\n", name, r.x, r.y, r.w, r.h);
}

const char * GetExtension(const char * path)
{
    const char * dot = strrchr(path, '.'); // find the last dot
    if ( dot == NULL || dot == path ) {
        return "";
    }

    return dot + 1;
}

FILE * OpenFile(const char * file_name, const char * mode)
{
    FILE * file = fopen(file_name, mode);

    if ( file == NULL ) {
        Error("could not open %s", file_name);
    }

    return file;
}

// TODO: check portability
float ProgramTime(void)
{
    struct timeval timeval;
    static long seconds = 0;

    gettimeofday(&timeval, NULL);
    if ( seconds == 0 ) {
        seconds = timeval.tv_sec;
        return timeval.tv_usec / 1000000.0f;
    }

    return (timeval.tv_sec - seconds) + timeval.tv_usec / 1000000.0f;
}

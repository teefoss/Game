#include "text.h"

#include "genlib.h"

#include "fonts/cp437_8x8.h"
#include "fonts/cp437_8x16.h"
#include "fonts/atari_4x8.h"
#include "fonts/nes_16x16.h"

#include <stdarg.h>

typedef struct {
    int width;
    int height;
    const unsigned char * data;
} font_info_t;

static font_info_t info[] = {
    [FONT_ATARI_4X8]    = {  4,  8, atari_4x8   },
    [FONT_CP437_8X8]    = {  8,  8, cp437_8x8   },
    [FONT_CP437_8X16]   = {  8, 16, cp437_8x16  },
    [FONT_NES_16X16]    = { 16, 16, nes_16x16   },
};

static SDL_Renderer * renderer; // unowned reference, do not destroy

// settings for text rendering
static font_t   font        = FONT_CP437_8X16;
static float    scaleX      = 1.0f; // horizontal draw scale
static float    scaleY      = 1.0f; // vertical draw scale
static int      tabSize     = 4;


#pragma mark - PUBLIC FUNCTIONS

void SetTextRenderer(SDL_Renderer * _renderer)
{
    renderer = _renderer;
}

void SetFont(font_t _font)
{
    font = _font;
}

void SetTextScale(float x, float y)
{
    scaleX = x;
    scaleY = y;
}

void SetTabSize(int size)
{
    tabSize = size;
}

int CharWidth(void)
{
    return info[font].width * scaleX;
}

int CharHeight(void)
{
    return info[font].height * scaleY;
}

void PutChar(int x, int y, unsigned char character)
{
    if ( renderer == NULL ) {
        Error("no font renderer is set, use SetFontRenderer()");
    }

    // save the current render scale
    float oldScaleX;
    float oldScaleY;
    SDL_RenderGetScale(renderer, &oldScaleX, &oldScaleY);

    SDL_RenderSetScale(renderer, scaleX, scaleY);

    // scale drawing but not coordinates
    const int unscaledX = (float)x / scaleX;
    const int unscaledY = (float)y / scaleY;

    const int w = info[font].width;
    const int h = info[font].height;
    const int bytesPerChar = (w * h) / 8;
    const u8 * data = &info[font].data[character * bytesPerChar];
    int bit = 7;

    for ( int row = 0; row < h; row++ ) {
        for ( int col = 0; col < w; col++ ) {

            if ( *data & (1 << bit) ) {
                SDL_RenderDrawPoint(renderer, unscaledX + col, unscaledY + row);
            }

            if ( --bit < 0 ) {
                ++data;
                bit = 7;
            }
        }
    }

    // restore previous render scale
    SDL_RenderSetScale(renderer, oldScaleX, oldScaleY);
}

void Print(int x, int y, const char * format, ...)
{
    va_list args[2];
    va_start(args[0], format);
    va_copy(args[1], args[0]);

    int len = vsnprintf(NULL, 0, format, args[0]);
    char * buffer = calloc(len + 1, sizeof(*buffer));
    vsnprintf(buffer, len + 1, format, args[1]);
    va_end(args[0]);
    va_end(args[1]);

    const char * c = buffer;
    int x1 = x;
    int y1 = y;
    int w = info[font].width * scaleX;
    int h = info[font].height * scaleY;

    while ( *c ) {
        switch ( *c ) {
            case '\n':
                y1 += h;
                x1 = x;
                break;
            case '\t':
                while ( (++x1 - x) % (tabSize * w) != 0 )
                    ;
                break;
            default:
                PutChar(x1, y1, *c);
                x1 += w;
                break;
        }

        c++;
    }

    free(buffer);
}

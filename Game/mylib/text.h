// -----------------------------------------------------------------------------
//  Text Library
//
//  Rendering of bitmap font text.
//  TODO: handle sprite sheet fonts?
// -----------------------------------------------------------------------------
#ifndef __TEXT_H__
#define __TEXT_H__

#include <SDL.h>

typedef enum {
    FONT_ATARI_4X8,
    FONT_CP437_8X16,
    FONT_CP437_8X8,
    FONT_NES_16X16
} font_t;

/// Select which renderer to use when rendering text.
///
/// In most cases this should be called once at the
/// the start of the program.
void SetTextRenderer(SDL_Renderer * renderer);

void SetFont(font_t font);
void SetTextScale(float x, float y);
void SetTabSize(int size);

/// Get the current font character scaled width in pixels.
int CharWidth(void);

/// Get the current font character scaled height in pixels.
int CharHeight(void);

/// Render ASCII character at pixel coordinate (x, y) using current renderer
/// color.
void PutChar(int x, int y, unsigned char character);

///  Render string at pixel coordinate (x, y) using current renderer color.
///
///  The control characters \n and \t are handled as expected.
void Print(int x, int y, const char * format, ...);

#endif /* __TEXT_H__ */

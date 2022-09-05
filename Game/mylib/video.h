// -----------------------------------------------------------------------------
// Video Library
//
// Window and renderer stuff.
// -----------------------------------------------------------------------------
#ifndef __VIDEO_H__
#define __VIDEO_H__

#include "genlib.h"
#include <SDL2/SDL.h>

typedef struct {
    const char * title;
    int x;
    int y;
    int width;
    int height;
    int flags;

    // TODO: some of this is in progress...
    struct {
        int flags;
        int outputWidth;
        int outputHeight;
        int logicalWidth;
        int logicalHeight;
        float scaleX;
        float scaleY;
    } render;
} windowInfo_t;

typedef enum {
    // old-school fullscreen, change desktop resolution
    FULLSCREEN = SDL_WINDOW_FULLSCREEN,
    // fake fullscreen, just make the window fill the screen
    // (favor this for 2D games)
    DESKTOP = SDL_WINDOW_FULLSCREEN_DESKTOP,
} fullscreen_t;

extern SDL_Renderer * renderer;

/// Initialize window and renderer with options specified in `info`.
void InitWindow(windowInfo_t info);

/// Get current information about the window.
windowInfo_t WindowInfo(void);

void GoFullscreen(fullscreen_t mode);
void GoWindowed(void);
void ToggleFullscreen(fullscreen_t mode);

void DrawCircle (int x0, int y0, int radius);

/// Clear the rendering target with current draw color.
inline void
Clear(void)
{
    SDL_RenderClear(renderer);
}

/// Present any rendering that was done since the previous call.
inline void
Present(void)
{
    SDL_RenderPresent(renderer);
}

/// Draw a rectangle outline with the current draw color.
inline void
DrawRect(SDL_Rect rect)
{
    SDL_RenderDrawRect(renderer, &rect);
}

/// Draw a filled rectangle with the current draw color.
inline void
FillRect(SDL_Rect rect)
{
    SDL_RenderFillRect(renderer, &rect);
}

/// Draw a point at pixel coordinates x, y.
inline void
DrawPoint(int x, int y)
{
    SDL_RenderDrawPoint(renderer, x, y);
}

/// Set the draw color.
inline void
SetRGBA(u8 r, u8 g, u8 b, u8 a)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

/// Set the draw color.
inline void
SetColor(SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

/// Set the draw color.
inline void
SetGray(u8 gray)
{
    SDL_SetRenderDrawColor(renderer, gray, gray, gray, 255);
}

/// Copy a portion of the texture to current rendering target.
/// - Parameter src: The portion of the texture to be drawn, or `NULL` to
///   draw the entire texture.
/// - Parameter dst: the location with the rending target to draw to, or `NULL`
///   to draw to entire target.
inline void
DrawTexture(SDL_Texture * texture, SDL_Rect * src, SDL_Rect * dst)
{
    SDL_RenderCopy(renderer, texture, src, dst);
}

inline SDL_Texture *
CreateTexture(int w, int h)
{
    SDL_Texture * texture = SDL_CreateTexture
    (   renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        w, h );

    if ( texture == NULL ) {
        Error("could not create texture (%s)\n", SDL_GetError());
    }

    return texture;
}

#endif /* __VIDEO_H__ */

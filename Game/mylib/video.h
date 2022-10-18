// -----------------------------------------------------------------------------
// Video Library
//
// Window and renderer stuff.
// -----------------------------------------------------------------------------
#ifndef __VIDEO_H__
#define __VIDEO_H__

#include "genlib.h"
#include <SDL.h>

typedef struct {            // default value:
    const char * title;     // ""
    int x;                  // SDL_WINDOWPOS_CENTERED
    int y;                  // SDL_WINDOWPOS_CENTERED
    int width;              // 640
    int height;             // 480
    int window_flags;       // 0
    int render_flags;       // 0
} window_info_t;

typedef enum {
    // old-school fullscreen, change desktop resolution
    FULLSCREEN = SDL_WINDOW_FULLSCREEN,
    // fake fullscreen, just make the window fill the screen
    // (favor this for 2D games)
    DESKTOP = SDL_WINDOW_FULLSCREEN_DESKTOP,
} fullscreen_t;

extern SDL_Window * window;
extern SDL_Renderer * renderer;

/// Initialize window and renderer with options specified in `info`.
/// - Parameter info: `NULL` or Zero values indicate default values
///   should be used.
void InitWindow(window_info_t * info);

/// Get current information about the window.
window_info_t WindowInfo(void);

void GoFullscreen(fullscreen_t mode);
void GoWindowed(void);
void ToggleFullscreen(fullscreen_t mode);

void V_DrawCircle (int x0, int y0, int radius);

/// Clear the rendering target with current draw color.
inline void V_Clear(void)
{
    SDL_RenderClear(renderer);
}

/// Present any rendering that was done since the previous call.
inline void V_Present(void)
{
    SDL_RenderPresent(renderer);
}

/// Draw a rectangle outline with the current draw color.
inline void V_DrawRect(SDL_Rect * rect)
{
    SDL_RenderDrawRect(renderer, rect);
}

/// Draw a filled rectangle with the current draw color.
inline void V_FillRect(SDL_Rect * rect)
{
    SDL_RenderFillRect(renderer, rect);
}

/// Draw a point at pixel coordinates x, y.
inline void V_DrawPoint(int x, int y)
{
    SDL_RenderDrawPoint(renderer, x, y);
}

/// Set the draw color.
inline void V_SetRGBA(u8 r, u8 g, u8 b, u8 a)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

inline void V_SetRGB(u8 r, u8 g, u8 b)
{
    V_SetRGBA(r, g, b, SDL_ALPHA_OPAQUE);
}

/// Set the draw color.
inline void V_SetColor(SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

/// Set the draw color.
inline void V_SetGray(u8 gray)
{
    SDL_SetRenderDrawColor(renderer, gray, gray, gray, 255);
}

/// Copy a portion of the texture to current rendering target.
/// - Parameter src: The portion of the texture to be drawn, or `NULL` to
///   draw the entire texture.
/// - Parameter dst: the location with the rending target to draw to, or `NULL`
///   to draw to entire target.
inline void V_DrawTexture(SDL_Texture * texture, SDL_Rect * src, SDL_Rect * dst)
{
    SDL_RenderCopy(renderer, texture, src, dst);
}

inline void V_DrawTextureFlip
(   SDL_Texture * texture,
    SDL_Rect * src,
    SDL_Rect * dst,
    SDL_RendererFlip flip )
{
    SDL_RenderCopyEx(renderer, texture, src, dst, 0.0, NULL, flip);
}

/// Create an SDL_Texture with that can be used as a rendering target.
SDL_Texture * V_CreateTexture(int w, int h);

#endif /* __VIDEO_H__ */

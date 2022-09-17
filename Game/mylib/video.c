#include "video.h"

SDL_Window * window;
SDL_Renderer * renderer;

static void CleanUp(void)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void InitWindow(window_info_t * info) {
    if ( !SDL_WasInit(SDL_INIT_VIDEO) ) {
        if ( SDL_InitSubSystem(SDL_INIT_VIDEO) != 0 ) {
            Error("could not init SDL video subsystem: %s", SDL_GetError());
        }
    }

    atexit(CleanUp);

    window_info_t _info;
    if ( info == NULL ) {
        _info = (window_info_t){
            .title = "",
            .x = SDL_WINDOWPOS_CENTERED,
            .y = SDL_WINDOWPOS_CENTERED,
            .width = 640,
            .height = 480,
            .window_flags = 0,
            .render_flags = 0
        };
    } else {
        _info = *info;
    }

    window = SDL_CreateWindow
    (   _info.title,
        _info.x == 0 ? SDL_WINDOWPOS_CENTERED : _info.x,
        _info.y == 0 ? SDL_WINDOWPOS_CENTERED : _info.y,
        _info.width == 0 ? 640 : _info.width,
        _info.height == 0 ? 480 : _info.height,
        _info.window_flags );

    if ( window == NULL ) {
        Error("could not create window: %s", SDL_GetError());
    }

    renderer = SDL_CreateRenderer(window, -1, _info.render_flags);

    if ( renderer == NULL ) {
        Error("could not create renderer: %s", SDL_GetError());
    }
}

window_info_t WindowInfo(void)
{
    window_info_t info;
    info.title = SDL_GetWindowTitle(window);
    SDL_GetWindowPosition(window, &info.x, &info.y);
    SDL_GetWindowSize(window, &info.width, &info.height);
    info.window_flags = SDL_GetWindowFlags(window);

    return info;
}

void GoFullscreen(fullscreen_t mode)
{
    SDL_SetWindowFullscreen(window, mode);
}

void GoWindowed(void)
{
    SDL_SetWindowFullscreen(window, 0);
}

static float scaleX;
static float scaleY;

void ToggleFullscreen(fullscreen_t mode)
{
    if ( SDL_GetWindowFlags(window)
        & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP) ) {
        if ( mode == DESKTOP ) {
            // restore saved scale
            SDL_RenderSetScale(renderer, scaleX, scaleY);
        }
        GoWindowed();
    } else {
        if ( mode == DESKTOP ) {
//            SDL_GetWindowSize(window, &windowWidth, &windowHeight);
//            SDL_RenderGetScale(renderer, &scaleX, &scaleY);
//
//            int displayIndex = SDL_GetWindowDisplayIndex(window);
//            SDL_DisplayMode displayMode;
//            SDL_GetDesktopDisplayMode(displayIndex, &displayMode);
//            float scale = (float)displayMode.h / (float)windowHeight;
//            print(scale);
//            SDL_RenderSetScale(renderer, scale, scale);
        }

        GoFullscreen(mode);
    }
}

// midpoint circle algorithm
void DrawCircle (int x0, int y0, int radius)
{
    int f = 1 - radius;
    int ddF_x = 0;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;

    SDL_RenderDrawPoint(renderer, x0, y0 + radius);
    SDL_RenderDrawPoint(renderer, x0, y0 - radius);
    SDL_RenderDrawPoint(renderer, x0 + radius, y0);
    SDL_RenderDrawPoint(renderer, x0 - radius, y0);

    while ( x < y ) {

        if ( f >= 0 ) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }

        x++;
        ddF_x += 2;
        f += ddF_x + 1;

        SDL_RenderDrawPoint(renderer, x0 + x, y0 + y);
        SDL_RenderDrawPoint(renderer, x0 - x, y0 + y);
        SDL_RenderDrawPoint(renderer, x0 + x, y0 - y);
        SDL_RenderDrawPoint(renderer, x0 - x, y0 - y);
        SDL_RenderDrawPoint(renderer, x0 + y, y0 + x);
        SDL_RenderDrawPoint(renderer, x0 - y, y0 + x);
        SDL_RenderDrawPoint(renderer, x0 + y, y0 - x);
        SDL_RenderDrawPoint(renderer, x0 - y, y0 - x);
    }
}

SDL_Texture * CreateTexture(int w, int h)
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

extern inline void Clear(void);
extern inline void Present(void);
extern inline void DrawRect(SDL_Rect rect);
extern inline void FillRect(SDL_Rect rect);
extern inline void DrawPoint(int x, int y);
extern inline void SetRGBA(u8 r, u8 g, u8 b, u8 a);
extern inline void SetColor(SDL_Color color);
extern inline void SetGray(u8 gray);
extern inline void DrawTexture(SDL_Texture * t, SDL_Rect * src, SDL_Rect * dst);
extern inline void DrawTextureFlip
(   SDL_Texture * texture,
    SDL_Rect * src,
    SDL_Rect * dst,
    SDL_RendererFlip flip );

#include "video.h"

static SDL_Window * window;
SDL_Renderer * renderer;

static void CleanUp(void)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void InitWindow(windowInfo_t info) {
    if ( !SDL_WasInit(SDL_INIT_VIDEO) ) {
        if ( SDL_InitSubSystem(SDL_INIT_VIDEO) != 0 ) {
            Error("could not init SDL video subsystem: %s", SDL_GetError());
        }
    }

    atexit(CleanUp);

    window = SDL_CreateWindow
    (   info.title,
        info.x == 0 ? SDL_WINDOWPOS_CENTERED : info.x,
        info.y == 0 ? SDL_WINDOWPOS_CENTERED : info.y,
        info.width == 0 ? 640 : info.width,
        info.height == 0 ? 480 : info.height,
        info.flags );

    if ( window == NULL ) {
        Error("could not create window: %s", SDL_GetError());
    }

    renderer = SDL_CreateRenderer(window, -1, info.render.flags);

    if ( renderer == NULL ) {
        Error("could not create renderer: %s", SDL_GetError());
    }

    if ( info.render.logicalWidth && info.render.logicalHeight ) {
        SDL_RenderSetLogicalSize
        (   renderer,
            info.render.logicalWidth,
            info.render.logicalHeight );
    } else {
        SDL_RenderSetLogicalSize(renderer, info.width, info.height);
    }

    if ( info.render.scaleX || info.render.scaleY ) {
        SDL_RenderSetScale
        (   renderer,
            info.render.scaleX ? info.render.scaleX : 1.0f,
            info.render.scaleY ? info.render.scaleY : 1.0f );
    }
}

windowInfo_t WindowInfo(void)
{
    windowInfo_t info;
    info.title = SDL_GetWindowTitle(window);
    SDL_GetWindowPosition(window, &info.x, &info.y);
    SDL_GetWindowSize(window, &info.width, &info.height);
    info.flags = SDL_GetWindowFlags(window);
    //info.render.flags = ?
    SDL_GetRendererOutputSize
    (   renderer,
        &info.render.outputWidth,
        &info.render.outputHeight );
    SDL_RenderGetLogicalSize
    (   renderer,
        &info.render.logicalWidth,
        &info.render.logicalHeight );
    SDL_RenderGetScale(renderer, &info.render.scaleX, &info.render.scaleY);

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
static int windowWidth;
static int windowHeight;

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


extern inline void Clear(void);
extern inline void Present(void);
extern inline void DrawRect(SDL_Rect rect);
extern inline void FillRect(SDL_Rect rect);
extern inline void DrawPoint(int x, int y);
extern inline void SetRGBA(u8 r, u8 g, u8 b, u8 a);
extern inline void SetColor(SDL_Color color);
extern inline void SetGray(u8 gray);
extern inline void DrawTexture(SDL_Texture * t, SDL_Rect * src, SDL_Rect * dst);

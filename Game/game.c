//
//  game.c
//  Game
//
//  Created by Thomas Tillistrand on 9/5/22.
//

#include "game.h"
#include "w_world.h"

#include "mylib/genlib.h"
#include "mylib/video.h"
#include "mylib/text.h"
#include "mylib/texture.h"
#include "mylib/input.h"

#include <SDL.h>

#define DRAW_SCALE  3

#define FPS 60.0f

// Debug info, toggled by function keys.
static bool show_geometry;
static bool show_world;
static bool show_debug_info;

static int frame;
static int frame_ms;
static int render_ms;

static bool DoFrame(world_t * world, float dt)
{
    SDL_Event event;
    while ( SDL_PollEvent(&event) ) {
        switch ( event.type ) {
            case SDL_QUIT:
                return false;
            case SDL_KEYDOWN:
                switch ( event.key.keysym.sym ) {
                    case SDLK_F1:
                        show_debug_info = !show_debug_info;
                        break;
                    case SDLK_F2:
                        show_world = !show_world;
                        break;
                    case SDLK_F3:
                        show_geometry = !show_geometry;
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    UpdateWorld(world, dt);

    SetGray(0);
    Clear();

    int render_start = SDL_GetTicks();
    RenderWorld(world);
    render_ms = SDL_GetTicks() - render_start;

    // debug move world camera:
#if 0
    float camera_movement = 4.0f * dt; // 2 tiles per second
    if ( keyboard[SDL_SCANCODE_UP] )    world->camera.y -= camera_movement;
    if ( keyboard[SDL_SCANCODE_DOWN] )  world->camera.y += camera_movement;
    if ( keyboard[SDL_SCANCODE_LEFT] )  world->camera.x -= camera_movement;
    if ( keyboard[SDL_SCANCODE_RIGHT] ) world->camera.x += camera_movement;
#endif

    // debug geometry:
    if ( show_geometry ) {
        SetRGBA(255, 0, 0, 255);
        SDL_RenderDrawLine(renderer, GAME_WIDTH / 2, 0, GAME_WIDTH / 2, GAME_HEIGHT);
        SDL_RenderDrawLine(renderer, 0, GAME_HEIGHT / 2, GAME_WIDTH, GAME_HEIGHT / 2);

        for ( int y = 0; y <= GAME_HEIGHT / TILE_SIZE; y++ ) {
            for ( int x = 0; x <= GAME_WIDTH / TILE_SIZE; x++ ) {
                DrawPoint(x * TILE_SIZE, y * TILE_SIZE);
            }
        }
    }

    // draw debug world texture
    if ( show_world ) {
        SDL_Rect dst = { 0, 0, GAME_HEIGHT, GAME_HEIGHT };
        DrawTexture(world->debug_texture, NULL, &dst);
    }

    // debug text:
    if ( show_debug_info ) {
        SetGray(255);
        int h = CharHeight();
        int row = 0;
        Print(0, row++ * h, "Frame time: %2d ms", frame_ms);
        Print(0, row++ * h, "- Render time: %2d ms", render_ms);
        Print(0, row++ * h, "Camera Tile: %.2f, %.2f", world->camera.x, world->camera.y);
    }

    Present();
    frame++;

    return true;
}

static void GameLoop(world_t * world)
{
    float old_time = ProgramTime();
    while ( true ) {
        float new_time = ProgramTime();
        float dt = new_time - old_time;

        if ( dt < 1.0f / FPS ) {
            SDL_Delay(1);
            continue;
        }

        int frame_start = SDL_GetTicks();
        if ( !DoFrame(world, dt) ) {
            return;
        }
        frame_ms = SDL_GetTicks() - frame_start;

        old_time = new_time;
    }
}

void GameMain(void)
{
    // system init

    window_info_t info = {
        .width = GAME_WIDTH * DRAW_SCALE,
        .height = GAME_HEIGHT * DRAW_SCALE
    };
    InitWindow(&info);
    SDL_RenderSetLogicalSize(renderer, GAME_WIDTH, GAME_HEIGHT);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    LoadTextures("Assets", "png");
    InitInput();
    SetTextRenderer(renderer);
    SetTextScale(2.0f, 2.0f);

    // game / world init
    world_t * world = CreateWorld();

    // debug: check things aren't getting too big
    printf("tile data size: %zu bytes\n", sizeof(world->tiles[0]));
    printf("world data size: %zu bytes\n", sizeof(*world));

    GameLoop(world);
    
    // clean up
    DestroyWorld(world);
}

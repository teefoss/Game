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


typedef struct {
    bool (* handle_event)(const SDL_Event * event);
    void (* update)(world_t *, float);
    void (* render)();
} game_state_t;

typedef struct {
    game_state_t state;
    int ticks;
} game_t;

// Debug info, toggled by function keys.
static bool show_geometry;
static bool show_world;
static bool show_debug_info;

static int frame;
static int frame_ms;
static int render_ms;
static int update_ms;

game_state_t game_play = {
    .handle_event = NULL,
    .update = UpdateWorld,
    .render = RenderWorld,
};

static bool DoFrame(game_t * game, world_t * world, float dt)
{
    SDL_Event event;
    while ( SDL_PollEvent(&event) ) {
        if ( game->state.handle_event ) {
            if ( game->state.handle_event(&event) ) {
                continue;
            }
        }

        // The current game state did not handle this event. Handle any
        // universal events, like quit:
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
                        if ( show_world ) {
                            UpdateDebugMap
                            (   world->tiles,
                                &world->debug_texture,
                                world->camera );
                        }
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

    //UpdateWorld(world, dt);
    int update_start = SDL_GetTicks();
    game->state.update(world, dt);
    update_ms = SDL_GetTicks() - update_start;

    SetGray(0);
    Clear();

    int render_start = SDL_GetTicks();
    //RenderWorld(world);
    game->state.render(world);
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
        Print(0, row++ * h, "- Update time: %2d ms", update_ms);
        Print(0, row++ * h, "Camera Tile: %.2f, %.2f", world->camera.x, world->camera.y);
    }

    Present();
    frame++;

    return true;
}

static void GameLoop(game_t * game, world_t * world)
{
    float old_time = ProgramTime();
    while ( true ) {
        float new_time = ProgramTime();
        float dt = new_time - old_time;

        if ( dt < 1.0f / FPS ) {
            SDL_Delay(1);
            continue;
        }
        if ( dt > 0.05f ) {
            dt = 0.05;
        }
        //dt = 1.0f / FPS;

        int frame_start = SDL_GetTicks();
        if ( !DoFrame(game, world, dt) ) {
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

    game_t * game = calloc(1, sizeof(*game));
    game->state = game_play;
    world_t * world = CreateWorld();

    // debug: check things aren't getting too big
    printf("tile data size: %zu bytes\n", sizeof(world->tiles[0]));
    printf("world data size: %zu bytes\n", sizeof(*world));

    GameLoop(game, world);
    
    // clean up
    DestroyWorld(world);
    free(game);
}

//
//  game.c
//  Game
//
//  Created by Thomas Tillistrand on 9/5/22.
//

#include "game.h"
#include "w_world.h"
#include "m_debug.h"

#include "mylib/genlib.h"
#include "mylib/video.h"
#include "mylib/text.h"
#include "mylib/texture.h"
#include "mylib/input.h"

#include <SDL.h>

typedef struct {
    bool (* handle_event)(const SDL_Event * event);
    void (* update)(world_t *, float);
    void (* render)();
} game_state_t;

typedef struct {
    game_state_t state;
    int ticks;
} game_t;


bool GameHandleEvent(const SDL_Event * event);

game_state_t game_play = {
    .handle_event = GameHandleEvent,
    .update = UpdateWorld,
    .render = RenderWorld,
};

bool GameHandleEvent(const SDL_Event * event)
{
    switch ( event->type ) {
        default:
            break;
    }

    return false;
}

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
                                &world->debug_map,
                                world->camera );
                        }
                        break;
                    case SDLK_F3:
                        show_geometry = !show_geometry;
                        break;
                    case SDLK_RIGHT:
                        world->clock += HOUR_TICKS / 2;
                        break;
                    case SDLK_LEFT:
                        world->clock -= HOUR_TICKS / 2;
                        if ( world->clock < 0 ) {
                            world->clock += DAY_LENGTH_TICKS;
                        }
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    game->state.update(world, dt);

    SetGray(0);
    Clear();
    game->state.render(world, show_geometry);
    DisplayDebugInfo(world);
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
        debug_dt = dt;

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
    printf("- tile data size: %zu bytes\n", sizeof(world->tiles[0]));
    printf("- world data size: %zu bytes\n", sizeof(*world));
    printf("- actor size: %zu bytes\n", sizeof(actor_t));

    GameLoop(game, world);
    
    // clean up
    DestroyWorld(world);
    free(game);
}

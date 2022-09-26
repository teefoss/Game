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

    static int max_render = 0;
    if ( frame > FPS * 2 && render_ms > max_render ) {
        max_render = render_ms;
        printf("max render time: %d\n", max_render);
    }

    if ( render_ms > 1000 / FPS ) {
        printf("frame %d: rending took %d ms!\n", frame, render_ms);
    }

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
        //dt = 1.0f / 60.0f;
//        if ( dt > 0.05f ) {
//            dt = 0.05;
//        }
        dt = FRAME_TIME_SEC;
        debug_dt = dt;

        int frame_start = SDL_GetTicks();
        if ( !DoFrame(game, world, dt) ) {
            return;
        }
        frame_ms = SDL_GetTicks() - frame_start;
        if ( frame_ms > 1000 / FPS ) {
            printf("frame took %d ms!\n", frame_ms);
        }

        old_time = new_time;
    }
}

void GameMain(void)
{
    // system init

    window_info_t info = {
        .width = GAME_WIDTH,
        .height = GAME_HEIGHT,
        .render_flags = SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC
    };
    InitWindow(&info);
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

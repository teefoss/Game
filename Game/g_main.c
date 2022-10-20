//
//  game.c
//  Game
//
//  Created by Thomas Tillistrand on 9/5/22.
//

#include "g_game.h"
#include "w_world.h"
#include "m_debug.h"

#include "mylib/genlib.h"
#include "mylib/video.h"
#include "mylib/texture.h"
#include "mylib/input.h"

#include <SDL.h>

static void G_DoFrame(game_t * game, float dt )
{
    IN_StartFrame(game->input_state);

    SDL_Event event;
    while ( SDL_PollEvent(&event) ) {

        IN_ProcessEvent(game->input_state, event);

        if ( UI_ProcessEvent(game, &event) ) {
            continue;
        }

        if ( G_ProcessEvent(game, &event) ) {
            continue;
        }

        if ( ProcessDebugEvent(game, &event) ) {
            continue;
        }

        // Handle any "universal" events
        switch ( event.type ) {
            case SDL_QUIT:
                game->is_running = false;
                return;
            case SDL_KEYDOWN:
                switch ( event.key.keysym.sym ) {
                    case SDLK_BACKSLASH:
                        ToggleFullscreen(DESKTOP);
                        break;
                    default:
                        break;
                }
                break;
            case SDL_WINDOWEVENT:
                switch ( event.window.event ) {
                    case SDL_WINDOWEVENT_RESIZED: {
                        float scale_x, scale_y;
                        SDL_RenderGetScale(renderer, &scale_x, &scale_y);
                        printf("scale after resize: %f, %f\n", scale_x, scale_y);
                        //V_SetTextScale(scale_x * DRAW_SCALE, scale_y * DRAW_SCALE);
                        break;
                    }
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    IN_Update(game->input_state);
    G_UpdateControlState(game->input_state);

    if ( !UI_ProcessInput(game) && !game->paused ) {
        G_Update(game, game->input_state, dt);
    }

    V_ClearRGB(0, 0, 0);
    G_Render(game);
    UI_Render(game);

    DisplayDebugInfo(game->world, IN_GetMousePosition(game->input_state));
    V_Refresh();

    // debug
    static int max_render = 0;
    if ( frame > FPS * 2 && render_ms > max_render ) {
        max_render = render_ms;
        printf("max render time: %d\n", max_render);
    }

    if ( render_ms > 1000 / FPS ) {
        printf("frame %d: rending took %d ms!\n", frame, render_ms);
    }

    frame++;
}

static void G_GameLoop(game_t * game)
{
    float old_time = ProgramTime();
    while ( game->is_running ) {
        float new_time = ProgramTime();
        float dt = new_time - old_time;

        if ( dt < 1.0f / FPS ) {
            SDL_Delay(1);
            continue;
        }

        dt = FRAME_TIME_SEC;
        debug_dt = dt;

        int frame_start = SDL_GetTicks();
        G_DoFrame(game, dt);
        frame_ms = SDL_GetTicks() - frame_start;

        if ( (float)frame_ms > 1000.0f / FPS ) {
            printf("frame took %d ms!\n", frame_ms);
        }

        old_time = new_time;
    }
}

void G_Main(void)
{
    window_info_t info = {
        .width = GAME_WIDTH,
        .height = GAME_HEIGHT,
        .window_flags = SDL_WINDOW_RESIZABLE,
        .render_flags = SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC
    };
    InitWindow(&info);
    SDL_DisableScreenSaver();
    SDL_RenderSetLogicalSize(renderer, GAME_WIDTH, GAME_HEIGHT);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    V_SetFont(FONT_CP437_8X8);
    V_SetTextScale(DRAW_SCALE, DRAW_SCALE);

    game_t * game = calloc(1, sizeof(*game));
    game->input_state = IN_Initialize();
    game->is_running = true;

    M_PushMenu(game, MENU_MAIN);
    UI_PushScreen(game, UI_MENU);
    game->state_top = -1;
    G_PushState(game, GAME_STATE_TITLE);

    // debug: check things aren't getting too big
    printf("- tile data size: %zu bytes\n", sizeof(tile_t));
    printf("- world data size: %zu bytes\n", sizeof(world_t));
    printf("- actor size: %zu bytes\n", sizeof(actor_t));

    G_GameLoop(game);

    // clean up
    FreeAllTextures();
    if ( game->world ) {
        DestroyWorld(game->world);
    }
    
    free(game->input_state);
    free(game);
    SDL_EnableScreenSaver();
}

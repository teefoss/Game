//
//  game.c
//  Game
//
//  Created by Thomas Tillistrand on 9/5/22.
//

#include "game.h"
#include "mylib/genlib.h"
#include "mylib/video.h"
#include <SDL.h>

// The internal resolution of the game.
// Aspect ratio is 16:9
// TODO: support non-16:9 ratios?
// This makes a visible area of 32 x 18 tiles (16 x 16 pixels)
#define GAME_WIDTH  512
#define GAME_HEIGHT 288
#define DRAW_SCALE  3

#define FPS 60.0f

bool DoFrame(float dt)
{
    SDL_Event event;
    while ( SDL_PollEvent(&event) ) {
        switch ( event.type ) {
            case SDL_QUIT:
                return false;
            default:
                break;
        }
    }

    return true;
}

static void GameLoop(void)
{
    float old_time = ProgramTime();
    while ( true ) {
        float new_time = ProgramTime();
        float dt = new_time - old_time;

        if ( dt < 1.0f / FPS ) {
            SDL_Delay(1);
            continue;
        }

        if ( !DoFrame(dt) ) {
            return;
        }

        old_time = new_time;
    }
}

void GameMain(void)
{
    // initialization
    window_info_t info = {
        .width = GAME_WIDTH * DRAW_SCALE,
        .height = GAME_HEIGHT * DRAW_SCALE
    };

    InitWindow(&info);
    SDL_RenderSetLogicalSize(renderer, GAME_WIDTH, GAME_HEIGHT);

    GameLoop();
    
    // clean up
}

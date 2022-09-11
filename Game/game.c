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
    window_info_t info = { 0 };
    InitWindow(&info);

    GameLoop();
    
    // clean up
}

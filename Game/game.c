//
//  game.c
//  Game
//
//  Created by Thomas Tillistrand on 9/5/22.
//

#include "mylib/video.h"
#include <SDL.h>
#include "game.h"

static void
GameLoop(void)
{
    while (1) {
        SDL_Delay(10);
    }
}

void
GameMain(void)
{
    // initialization
    windowInfo_t info = { 0 };
    InitWindow(info);
    SDL_PumpEvents();
    
    // game loop
    GameLoop();
    
    // clean up
}

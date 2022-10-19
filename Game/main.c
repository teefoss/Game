//
//  main.c
//  Game
//
//  Created by Thomas Foster on 9/5/22.
//

#include "g_game.h"

/*
 RESOURCES
 https://www.gamedeveloper.com/programming/dynamic-2d-character-lighting

 TODO: Merge text into video.c
 TODO: Debug info is UI screen
 TODO: Left stick to navigate menu
 TODO: stop using floating point clock
 TODO: update to use coord types instead of int x, y everywhere
 TODO: replace all Random() % ... with Chance()
 TODO: refine tile rendering, alla terrain type render function member
 TODO: more thorough frame time (what's using up 4-5 ms?)

 BUGS
 TODO: TF water edge not randomizing
 TODO: debug info effed while in fullscreen
 TODO: tile effect texture noise generation too slow
 */

int main(void)
{
    G_Main();
    return 0;
}

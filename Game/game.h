//
//  game.h
//  Game
//
//  Created by Thomas Tillistrand on 9/5/22.
//

#ifndef game_h
#define game_h

#include "mylib/sprite.h"
#include "mylib/vector.h"

#define FPS 60.0f

// The internal resolution of the game.
// Aspect ratio is 16:9
// TODO: support non-16:9 ratios?
// This makes a visible area of 30 x 16.875 tiles
// 4x this resolution = 1920x1080
#define DRAW_SCALE  3
#define GAME_WIDTH  (480 * DRAW_SCALE)
#define GAME_HEIGHT (270 * DRAW_SCALE)

void GameMain(void);

/// Draw sprite and scaled pixel coordinates with scaled size
void G_DrawSprite(sprite_t * sprite, vec2_t position, u8 frame);

#endif /* game_h */

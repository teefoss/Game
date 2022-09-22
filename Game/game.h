//
//  game.h
//  Game
//
//  Created by Thomas Tillistrand on 9/5/22.
//

#ifndef game_h
#define game_h

#define FPS 60.0f

// The internal resolution of the game.
// Aspect ratio is 16:9
// TODO: support non-16:9 ratios?
// This makes a visible area of 30 x 16.875 tiles
// 4x this resolution = 1920x1080
#define GAME_WIDTH  480
#define GAME_HEIGHT 270
#define DRAW_SCALE  3

void GameMain(void);

#endif /* game_h */
